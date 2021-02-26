 /*
* File: NetworkManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.12.26
*/

#include "MW.h"

namespace Milkweed {
	NetMessage::NetMessage(unsigned int ID,
		std::shared_ptr<NetConnection> owner) {
		header.ID = ID;
		this->owner = owner;
	}

	void NetConnection::init(TSQueue<NetMessage>* messagesIn,
		unsigned int maxMessageSize) {
		// Set the context, message in TSQ, and let the owner of the temporary
		// message be this connection
		m_messagesIn = messagesIn;
		m_maxMessageSize = maxMessageSize;
		m_tempMessage.owner = this->shared_from_this();
	}

	void NetConnection::connectToServer(
		const asio::ip::tcp::resolver::results_type& endpoints) {
		// Attempt to asynchronously connect to the socket to the server at the
		// given endpoint
		asio::async_connect(m_socket, endpoints,
			[this](std::error_code error,
				asio::ip::tcp::endpoint endpoint) {
				if (!error) {
					// The connection was successful, begin reading messages
					m_connected = true;
					readHeader();
				}
				else {
					disconnect();
				}
			}
		);
	}

	void NetConnection::connectToClient(unsigned int ID) {
		// If the connection is open set the ID number and read a header
		if (m_socket.is_open()) {
			m_ID = ID;
			m_connected = true;
			readHeader();
		}
	}

	bool NetConnection::isConnected() {
		if (!m_socket.is_open()) {
			m_connected = false;
		}

		return m_connected;
	}

	void NetConnection::send(const NetMessage& message) {
		asio::post(m_context,
			[this, message]() {
				// Add the messsage to the out queue and if it was empty before
				// adding the message tell ASIO to begin writing messages
				bool messagesOutEmpty = m_messagesOut.empty();
				m_messagesOut.pushBack(message);
				if (messagesOutEmpty) {
					writeHeader();
				}
			}
		);
	}

	void NetConnection::disconnect() {
		// If the socket is connected, then tell the ASIO context to close it
		// TODO: May need to fix the dereference here
		if (isConnected()) {
			m_connected = false;
			asio::post(m_context, [this]() { m_socket.close(); });
		}
	}

	void NetConnection::destroy() {
		// Ensure this connection is not connected
		disconnect();

		// Reset the context, the messages in TSQ, the ID of this connection,
		// and the max message size
		m_messagesIn = nullptr;
		m_ID = 0;
		m_maxMessageSize = 0;

		// Get rid of any messages waiting to be send and get rid of their
		// owner pointers
		for (unsigned int i = 0; i < m_messagesOut.size(); i++) {
			m_messagesOut.at(i).owner.reset();
		}
		m_messagesOut.clear();

		// Get rid of the contents of the temporary message
		m_tempMessage.owner.reset();
		m_tempMessage.header.ID = 0;
		m_tempMessage.header.size = 0;
		m_tempMessage.body.clear();
	}

	void NetConnection::readHeader() {
		asio::async_read(m_socket,
			asio::buffer(&m_tempMessage.header, sizeof(NetMessageHeader)),
			[this](std::error_code error, std::size_t length) {
				if (!error) {
					if (m_tempMessage.header.size == 0) {
						// There is no body, this message can be added without
						// one
						addMessageIn();
					}
					else {
						if (m_tempMessage.header.size <= m_maxMessageSize) {
							// There is a body to read for this message, read
							// it in from the network
							m_tempMessage.body.resize(
								m_tempMessage.header.size);
							readBody();
						}
						else {
							// The connection won't take this message because
							// its specified body size is too big, this is
							// likely because a client has connected which is
							// not the appropriate application
							disconnect();
						}
					}
				}
				else {
					// An error has occurred, we've been disconnected
					disconnect();
				}
			}
		);
	}

	void NetConnection::readBody() {
		asio::async_read(m_socket, asio::buffer(m_tempMessage.body.data(),
			m_tempMessage.body.size()),
			[this](std::error_code error, std::size_t length) {
				if (!error) {
					// Add the message to the incoming message queue
					addMessageIn();
				}
				else {
					// The socket has been disconnected
					disconnect();
				}
			}
		);
	}

	void NetConnection::addMessageIn() {
		// Push the temporary message onto the back of the messages in TSQ since
		// it already has this NetConnection as its owner
		m_messagesIn->pushBack(m_tempMessage);
		readHeader();
	}

	void NetConnection::writeHeader() {
		asio::async_write(m_socket, asio::buffer(&m_messagesOut.front().header,
			sizeof(NetMessageHeader)),
			[this](std::error_code error, std::size_t length) {
				if (!error) {
					// The data was written, check if the message has a body
					if (m_messagesOut.front().body.size() > 0) {
						// Write the body out
						writeBody();
					}
					else {
						// The message has no body, get rid of it
						m_messagesOut.popFront();
						// If there is another message to write, begin again at
						// its header
						if (!m_messagesOut.empty()) {
							writeHeader();
						}
					}
				}
				else {
					// The socket has been disconnected
					disconnect();
				}
			}
		);
	}

	void NetConnection::writeBody() {
		asio::async_write(m_socket,
			asio::buffer(m_messagesOut.front().body.data(),
				m_messagesOut.front().body.size()),
			[this](std::error_code error, std::size_t length) {
				if (!error) {
					// The body was written, get rid of it
					m_messagesOut.popFront();
					// If there are more messages to write, start at the next
					// one's header
					if (!m_messagesOut.empty()) {
						writeHeader();
					}
				}
				else {
					// Couldn't write the body, the socket has been disconnected
					disconnect();
				}
			}
		);
	}

	NetClient NetClient::m_instance;

	void NetClient::init(unsigned int maxMessageSize) {
		m_connection = std::make_shared<NetConnection>(m_context, m_socket);
		m_connection->init(&m_messagesIn, maxMessageSize);
	}

	void NetClient::connect(const std::string& address, unsigned int port) {
		try {
			// Attempt to resolve the host
			asio::ip::tcp::resolver resolver(m_context);
			asio::ip::tcp::resolver::results_type endpoints
				= resolver.resolve(address, std::to_string(port));

			// Attempt to make the connection
			m_connection->connectToServer(endpoints);

			// Attempt to start the ASIO thread
			m_ASIOThread = std::thread([this]() { m_context.run(); });
		}
		catch (std::exception& e) {
			// ASIO could not resolve the host
			const char* error = e.what();
		}
	}

	void NetClient::send(const NetMessage& message) {
		// Send the message via the connection and test if we're still connected
		if (isConnected()) {
			m_connection->send(message);
		}
		else {
			disconnect();
		}
	}

	void NetClient::disconnect() {
		// Disconnect the connection if it is connected
		if (isConnected()) {
			m_connection->disconnect();
		}

		// Stop the ASIO thread
		m_context.stop();
		if (m_ASIOThread.joinable()) {
			m_ASIOThread.join();
		}
	}

	void NetClient::destroy() {
		disconnect();
		m_connection->destroy();
		m_connection.reset();
		m_messagesIn.clear();
	}

	bool NetServer::init(unsigned int maxMessageSize) {
		m_log.init("mwlog/");

		m_maxMessageSize = maxMessageSize;

		try {
			// Issue a task for waiting for a connection to the ASIO context
			waitForConnection();

			// Start the context in its thread
			m_ASIOThread = std::thread([this]() { m_context.run(); });
		}
		catch (std::exception& e) {
			const char* error = e.what();
			return false;
		}

		return true;
	}

	bool NetServer::messageClient(std::shared_ptr<NetConnection> client,
		const NetMessage& message) {
		if (client) {
			if (client->isConnected()) {
				// The client exists and is connected, send the message
				client->send(message);
				return true;
			}
		}

		// The client either was passed as nullptr, or is not connected,
		// remove it
		onDisconnect(client);
		client.reset();
		m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), client),
			m_clients.end());
		return false;
	}

	void NetServer::messageAllClients(const NetMessage& message,
		std::shared_ptr<NetConnection> ignoredClient) {
		if (ignoredClient) {

		}
		bool invalidClientExists = false;
		// Send the message to all valid and connected clients
		for (std::shared_ptr<NetConnection> client : m_clients) {
			if (client) {
				if (client->isConnected()) {
					// The client is valid and connected
					if (client != ignoredClient) {
						// The client is not to be ignored, send the message
						// and go to the next clients
						client->send(message);
					}
					continue;
				}
			}

			// The client was invalid or not connected, remove it
			onDisconnect(client);
			client.reset();
			invalidClientExists = true;
		}

		if (invalidClientExists) {
			// Remove all nullptr connections from the list of clients if
			// we are required to do so
			m_clients.erase(std::remove(m_clients.begin(), m_clients.end(),
				nullptr), m_clients.end());
		}
	}

	void NetServer::update(int maxMessages) {
		// Process up to maxMessages messages
		int i = 0;
		while (i != maxMessages && !m_messagesIn.empty()) {
			NetMessage message = m_messagesIn.popFront();
			onMessage(message);
			i++;
		}
	}

	void NetServer::setMaxMessageSize(unsigned int maxMessageSize) {
		// Set the message size for all new connections
		m_maxMessageSize = maxMessageSize;
		// Set the message size for all existing connections
		for (std::shared_ptr<NetConnection> client : m_clients) {
			client->setMaxMessageSize(maxMessageSize);
		}
	}

	void NetServer::destroy() {
		// Stop the context and attempt to join its thread
		m_context.stop();
		if (m_ASIOThread.joinable()) {
			m_ASIOThread.join();
		}
	}

	void NetServer::waitForConnection() {
		m_acceptor.async_accept(
			[this](std::error_code error, asio::ip::tcp::socket socket) {
				if (!error) {
					// Accept a new client
					std::shared_ptr<NetConnection> client
						= std::make_shared<NetConnection>(m_context, socket);
					client->init(&m_messagesIn, m_maxMessageSize);
					client->connectToClient(m_currentID++);

					if (onConnect(client)) {
						// The program has decided to accept the client
						m_clients.push_back(client);
					}
				}

				// Issue new work to the ASIO context
				waitForConnection();
			}
		);
	}
}