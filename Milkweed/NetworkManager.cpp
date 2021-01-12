 /*
* File: NetworkManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.12.26.1854
*/

#include "NetworkManager.h"
#include "MW.h"

namespace MW {
	void NetConnection::init(LogManager* log, TSQueue<NetMessage>* messagesIn,
		unsigned int maxMessageSize) {
		// Set the context, message in TSQ, and let the owner of the temporary
		// message be this connection
		m_log = log;
		m_messagesIn = messagesIn;
		m_maxMessageSize = maxMessageSize;
		m_tempMessage.owner = this->shared_from_this();
		*m_log << "Initialized connection\n";
	}

	void NetConnection::connectToServer(
		const asio::ip::tcp::resolver::results_type& endpoints) {
		*m_log << "Connecting to server\n";
		// Attempt to asynchronously connect to the socket to the server at the
		// given endpoint
		asio::async_connect(m_socket, endpoints,
			[this](std::error_code error,
				asio::ip::tcp::endpoint endpoint) {
				if (!error) {
					// The connection was successful, begin reading messages
					*m_log << "Connection successful\n";
					m_connected = true;
					readHeader();
				}
				else {
					*m_log << "Connection failed\n";
					disconnect();
				}
			}
		);
	}

	void NetConnection::connectToClient(unsigned int ID) {
		*m_log << "Connecting to client with ID " << ID << "\n";
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
		*m_log << "Connection sending message with ID " << message.header.ID
			<< "\n";
		asio::post(m_context,
			[this, message]() {
				// Add the messsage to the out queue and if it was empty before
				// adding the message tell ASIO to begin writing messages
				bool messagesOutEmpty = m_messagesOut.empty();
				m_messagesOut.pushBack(message);
				if (messagesOutEmpty) {
					*m_log << "Messages out was empty\n";
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
			*m_log << "Disconnecting connection\n";
			asio::post(m_context, [this]() { m_socket.close(); });
		}
	}

	void NetConnection::destroy() {
		*m_log << "Destroying network connection\n";
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
					*m_log << "Read message header\n";
					if (m_tempMessage.header.size == 0) {
						// There is no body, this message can be added without
						// one
						addMessageIn();
					}
					else {
						if (m_tempMessage.header.size <= m_maxMessageSize) {
							// There is a body to read for this message, read
							// it in from the network
							*m_log << "Reading message\n";
							m_tempMessage.body.resize(
								m_tempMessage.header.size);
							readBody();
						}
						else {
							*m_log << "Message longer than max size\n";
							// The connection won't take this message because
							// its specified body size is too big, this is
							// likely because a client has connected which is
							// not the appropriate application
							disconnect();
						}
					}
				}
				else {
					*m_log << "Connection failed to read message header\n";
					// An error has occurred, we've been disconnected
					disconnect();
				}
			}
		);
	}

	void NetConnection::readBody() {
		*m_log << "Reading message body\n";
		asio::async_read(m_socket, asio::buffer(m_tempMessage.body.data(),
			m_tempMessage.body.size()),
			[this](std::error_code error, std::size_t length) {
				if (!error) {
					*m_log << "Add message to input queue\n";
					// Add the message to the incoming message queue
					addMessageIn();
				}
				else {
					*m_log << "Failed to read message body\n";
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
		*m_log << "Writing message header\n";
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
						*m_log << "No message body to write\n";
						// The message has no body, get rid of it
						m_messagesOut.popFront();
						// If there is another message to write, begin again at
						// its header
						if (!m_messagesOut.empty()) {
							*m_log << "Messages out queue not empty!\n";
							writeHeader();
						}
					}
				}
				else {
					*m_log << "Failed to write message header\n";
					// The socket has been disconnected
					disconnect();
				}
			}
		);
	}

	void NetConnection::writeBody() {
		*m_log << "Writing message body\n";
		asio::async_write(m_socket,
			asio::buffer(m_messagesOut.front().body.data(),
				m_messagesOut.front().body.size()),
			[this](std::error_code error, std::size_t length) {
				if (!error) {
					*m_log << "Wrote message body\n";
					// The body was written, get rid of it
					m_messagesOut.popFront();
					// If there are more messages to write, start at the next
					// one's header
					if (!m_messagesOut.empty()) {
						*m_log << "Messages out queue not empty!\n";
						writeHeader();
					}
				}
				else {
					*m_log << "Failed to write message body\n";
					// Couldn't write the body, the socket has been disconnected
					disconnect();
				}
			}
		);
	}

	void NetClient::init(unsigned int maxMessageSize) {
		App::LOG << "Initializing network client with message size "
			<< maxMessageSize << "\n";
		m_connection = std::make_shared<NetConnection>(m_context, m_socket);
		App::LOG << "Created connection\n";
		m_connection->init(&App::LOG, &m_messagesIn, maxMessageSize);
		App::LOG << "Initialized connection\n";
	}

	void NetClient::connect(const std::string& address, unsigned int port) {
		try {
			// Attempt to resolve the host
			asio::ip::tcp::resolver resolver(m_context);
			asio::ip::tcp::resolver::results_type endpoints
				= resolver.resolve(address, std::to_string(port));
			App::LOG << "Resolved IP address " << address << ":" << port
				<< "\n";

			// Attempt to make the connection
			m_connection->connectToServer(endpoints);

			// Attempt to start the ASIO thread
			m_ASIOThread = std::thread([this]() { m_context.run(); });
			
			App::LOG << "Started ASIO\n";
		}
		catch (std::exception& e) {
			// ASIO could not resolve the host
			const char* error = e.what();
			App::LOG << "Failed to connect: " << error << "\n";
		}
	}

	void NetClient::send(const NetMessage& message) {
		// Send the message via the connection and test if we're still connected
		if (isConnected()) {
			m_connection->send(message);
			App::LOG << "Sent message " << message.header.ID << "\n";
		}
		else {
			App::LOG << "Disconnected, could not send message "
				<< message.header.ID << "\n";
			disconnect();
		}
	}

	void NetClient::disconnect() {
		// Disconnect the connection if it is connected
		if (isConnected()) {
			App::LOG << "Disconnecting...\n";
			m_connection->disconnect();
		}

		App::LOG << "Stopping ASIO context\n";
		// Stop the ASIO thread
		m_context.stop();
		if (m_ASIOThread.joinable()) {
			m_ASIOThread.join();
		}
	}

	void NetClient::destroy() {
		App::LOG << "Destroying network client\n";
		disconnect();
		m_connection->destroy();
		m_connection.reset();
		m_messagesIn.clear();
	}

	bool NetServer::init(unsigned int maxMessageSize) {
		m_log.init();

		m_log << "Initialize network server\n";
		m_maxMessageSize = maxMessageSize;

		try {
			// Issue a task for waiting for a connection to the ASIO context
			waitForConnection();

			// Start the context in its thread
			m_ASIOThread = std::thread([this]() { m_context.run(); });
			m_log << "Started ASIO\n";
		}
		catch (std::exception& e) {
			const char* error = e.what();
			m_log << "Failed to start server: " << std::string(error) << "\n";
			return false;
		}

		return true;
	}

	bool NetServer::messageClient(std::shared_ptr<NetConnection> client,
		const NetMessage& message) {
		m_log << "Sending message to client " << client->getID() << " with ID "
			<< message.header.ID << "\n";
		if (client) {
			if (client->isConnected()) {
				m_log << "Client connected, sending\n";
				// The client exists and is connected, send the message
				client->send(message);
				return true;
			}
		}

		m_log << "The client was disconnected, removing\n";
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
		m_log << "Sending message with ID " << message.header.ID
			<< " to all clients ";
		if (ignoredClient) {
			m_log << "except client " << ignoredClient->getID() << "\n";
		}
		bool invalidClientExists = false;
		// Send the message to all valid and connected clients
		for (std::shared_ptr<NetConnection> client : m_clients) {
			if (client) {
				if (client->isConnected()) {
					// The client is valid and connected
					if (client != ignoredClient) {
						m_log << "Client " << client->getID() << " is connected"
							<< " and not ignored\n";
						// The client is not to be ignored, send the message
						// and go to the next clients
						client->send(message);
					}
					continue;
				}
			}

			m_log << "Client " << client->getID() << " was disconnected, "
				<< "marking for removal\n";
			// The client was invalid or not connected, remove it
			onDisconnect(client);
			client.reset();
			invalidClientExists = true;
		}

		if (invalidClientExists) {
			m_log << "Removing disconnected clients\n";
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
			m_log << "Processing message " << message.header.ID << "\n";
			onMessage(message);
			i++;
		}
	}

	void NetServer::destroy() {
		m_log << "Stopping ASIO, destroying network server\n";
		// Stop the context and attempt to join its thread
		m_context.stop();
		if (m_ASIOThread.joinable()) {
			m_ASIOThread.join();
		}
	}

	void NetServer::waitForConnection() {
		m_log << "Awaiting new client connection...\n";
		m_acceptor.async_accept(
			[this](std::error_code error, asio::ip::tcp::socket socket) {
				if (!error) {
					m_log << "Connected new client at "
						<< socket.remote_endpoint() << "\n";
					// Accept a new client
					std::shared_ptr<NetConnection> client
						= std::make_shared<NetConnection>(m_context, socket);
					client->init(&m_log, &m_messagesIn, m_maxMessageSize);
					client->connectToClient(m_currentID++);

					if (onConnect(client)) {
						// The program has decided to accept the client
						m_clients.push_back(client);
						m_log << "Accepted new client with ID "
							<< client->getID() << "\n";
					}
				}

				// Issue new work to the ASIO context
				waitForConnection();
			}
		);
	}
}