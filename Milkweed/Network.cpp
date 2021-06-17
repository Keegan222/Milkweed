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
						// Push a connected message to the client
						NetMessage message(NetMessageTypes::CONNECTED,
							this->shared_from_this());
						m_messagesIn->pushBack(message);

						// Begin receiving messages from the network
						readHeader();
					}
					else {
						NetMessage msg;
						msg.header.ID = NetMessageTypes::FAILED;
						m_messagesIn->pushBack(msg);
						disconnect();
					}
			}
		);
	}

	void NetConnection::connectToClient(unsigned int ID) {
		// If the connection is open set the ID number
		if (m_socket.is_open()) {
			m_ID = ID;
			m_connected = true;
			m_serverOwned = true;

			// Begin receiving messages from the network
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
			// Notify the host of the disconnect
			m_connected = false;

			// Send the disconnected message if the NetConnection isn't owned
			// by a server
			if (!m_serverOwned && m_messagesIn != nullptr) {
				NetMessage message(NetMessageTypes::DISCONNECTED,
					this->shared_from_this());
				m_messagesIn->pushBack(message);
			}

			// Post the disconnect
			asio::post(m_context, [this]() { m_socket.close(); });
		}
	}

	void NetConnection::destroy() {
		// Ensure this connection is not connected
		disconnect();

		// Reset the context, the messages in TSQ, the ID of this connection,
		// and the max message size
		for (unsigned int i = 0; i < m_messagesIn->size(); i++) {
			m_messagesIn->at(i).owner.reset();
		}
		m_messagesIn->clear();
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

		MWLOG(Info, NetClient, "Initialized network client with max message ",
			"size ", maxMessageSize, " bytes");
	}

	void NetClient::connect(const std::string& address, unsigned int port) {
		try {
			MWLOG(Info, NetClient, "Connecting to ", address, " on port ", port);
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
			MWLOG(Warning, NetClient, "Network connection error: ", error);
			disconnect();
		}
	}

	void NetClient::send(const NetMessage& message) {
		// Send the message via the connection and test if we're still connected
		if (isConnected()) {
			m_connection->send(message);
		}
		else {
			MWLOG(Warning, NetClient, "Failed to send NetMessage");
			disconnect();
		}
	}

	void NetClient::disconnect() {
		MWLOG(Info, NetClient, "Disconnecting from server");
		// Disconnect the connection if it is connected
		m_connection->disconnect();

		// Stop the ASIO thread
		m_context.stop();
		if (m_ASIOThread.joinable()) {
			m_ASIOThread.join();
		}
		m_context.reset();
	}

	void NetClient::destroy() {
		MWLOG(Info, NetClient, "Destroying network client");
		disconnect();
		m_connection->destroy();
		m_connection.reset();
		m_messagesIn.clear();
	}

	bool NetServer::init(unsigned int maxMessageSize) {
		m_log.init("mwlog/");

		m_maxMessageSize = maxMessageSize;

		SERVERLOG(Info, "Initializing network server on port ", m_port,
			" with maximum message size ", maxMessageSize, " bytes");

		try {
			// Issue a task for waiting for a connection to the ASIO context
			waitForConnection();

			// Start the context in its thread
			m_ASIOThread = std::thread([this]() { m_context.run(); });
			SERVERLOG(Info, "Started ASIO listening thread");
		}
		catch (std::exception& e) {
			const char* error = e.what();
			SERVERLOG(Error, "Failed to start ASIO listening thread, error: ",
				error);
			return false;
		}

		return true;
	}

	bool NetServer::messageClient(std::shared_ptr<NetConnection> client,
		const NetMessage& message) {
		if (client != nullptr) {
			if (client->isConnected()) {
				// The client exists and is connected, send the message
				client->send(message);
				return true;
			}
		}

		// The client either was passed as nullptr, or is not connected,
		// remove it
		SERVERLOG(Info, "Client ", client->getID(), " has disconnected");
		onDisconnect(client);
		client.reset();
		m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), client),
			m_clients.end());
		return false;
	}

	void NetServer::messageAllClients(const NetMessage& message,
		std::shared_ptr<NetConnection> ignoredClient) {
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
				}
			}
		}
	}

	void NetServer::update(int maxMessages) {
		// Attempt to remove disconnected clients
		for (std::shared_ptr<NetConnection> client : m_clients) {
			if (client) {
				if (client->isConnected()) {
					// The client is still connected, move on
					continue;
				}
				else {
					// The client has disconnected, destroy it
					SERVERLOG(Info, "Client ", client->getID(),
						" has disconnected");
					onDisconnect(client);
					client->destroy();
				}
			}

			// Remove the invalid / disconnected client
			m_clients.erase(std::remove(m_clients.begin(), m_clients.end(),
				client), m_clients.end());
		}

		if (maxMessages == -1) {
			while (!m_messagesIn.empty()) {
				NetMessage message = m_messagesIn.popFront();
				onMessage(message);
			}
		}
		else {
			// Process up to maxMessages messages
			int i = 0;
			while (i <= maxMessages && !m_messagesIn.empty()) {
				NetMessage message = m_messagesIn.popFront();
				onMessage(message);
				i++;
			}
		}
	}

	void NetServer::setMaxMessageSize(unsigned int maxMessageSize) {
		// Set the message size for all new connections
		m_maxMessageSize = maxMessageSize;
		SERVERLOG(Info, "Updated maximum message size to ", maxMessageSize,
			" bytes");
		// Set the message size for all existing connections
		for (std::shared_ptr<NetConnection> client : m_clients) {
			client->setMaxMessageSize(maxMessageSize);
		}
	}

	void NetServer::destroy() {
		SERVERLOG(Info, "Stopping server, disconnecting all clients and ",
			"stopping ASIO listening thread");

		// Disconnect all clients
		for (std::shared_ptr<NetConnection> client : m_clients) {
			client->disconnect();
			client.reset();
		}
		m_clients.clear();

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

					SERVERLOG(Info, "Found new client connection");

					if (onConnect(client)) {
						// The program has decided to accept the client
						SERVERLOG(Info, "Accepted client, assigned ID ",
							client->getID());
						m_clients.push_back(client);
					}
				}
				else {
					SERVERLOG(Warning, "Failed to find new client connection");
				}

				// Issue new work to the ASIO context
				waitForConnection();
			}
		);
	}
}