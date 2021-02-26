/*
* File: NetworkManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.12.26
*/

#ifndef MW_NETWORK_H
#define MW_NETWORK_H

#include <ostream>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ts/buffer.hpp>

#include "Logging.h"

namespace Milkweed {
	// Forward declare NetConnection
	class NetConnection;

	/*
	* The header information of a NetMessage
	*/
	struct NetMessageHeader {
		// The ID number of this message, identifies the type of message
		unsigned int ID = 0;
		// The size of this messages in bytes, including the header
		unsigned int size = sizeof(NetMessageHeader);
	};

	/*
	* A message to pass over the internet between NetClient's and servers
	*/
	class NetMessage {
	public:
		// The connection which owns this message
		std::shared_ptr<NetConnection> owner = nullptr;
		// The header information of this message
		NetMessageHeader header;
		// The data contained in this message
		std::vector<char> body;

		/*
		* Construct this message with no owner connection
		*/
		NetMessage() {}
		/*
		* Construct this message with an ID number and a connection owner
		* (owner optional, nullptr by default)
		*/
		NetMessage(unsigned int ID,
			std::shared_ptr<NetConnection> owner = nullptr);
		/*
		* Override for the bitshift left operator to print header information
		* to an output stream
		*/
		friend std::ostream& operator<<(std::ostream& os, NetMessage& message) {
			os << "NetMessage with ID: " << message.header.ID << " and size: "
				<< message.header.size << " bytes";
			return os;
		}
		/*
		* Override for the bitshit left operator to push data from a variable
		* into the body of this message
		*/
		template<typename T>
		friend NetMessage& operator<<(NetMessage& message, T& var) {
			if (std::is_standard_layout<T>::value) {
				// Cache the current body size and resize the message
				size_t s = message.body.size();
				message.body.resize(message.body.size() + sizeof(T));
				// Copy the variable's data into the message body and recalculate
				// the size in the header
				std::memcpy(message.body.data() + s, &var, sizeof(T));
				message.header.size = (unsigned int)message.body.size();
			}

			// Return the message so that this operator can be chained
			return message;
		}
		/*
		* Override for the bitshift right operator to pull data from the body
		* of this message into a variable
		*/
		template<typename T>
		friend NetMessage& operator>>(NetMessage& message, T& var) {
			if (std::is_standard_layout<T>::value
				&& message.body.size() >= sizeof(T)) {
				// Cache the location in the message body data that the variable's
				// data begins
				size_t s = message.body.size() - sizeof(T);
				// Copy the data out of the message body and resize it
				std::memcpy(&var, message.body.data() + s, sizeof(T));
				message.body.resize(s);
				// Recalculate the message size in the header
				message.header.size = (unsigned int)message.body.size();
			}

			// Return the message so that this operator can be chained
			return message;
		}
	};

	/*
	* A thread-safe queue for storing network traffic, wrapper for a
	* std::deque with a mutex
	*/
	template <typename T>
	class TSQueue {
	public:
		/*
		* This class uses the default constructor for an std::deque
		*/
		TSQueue() = default;
		/*
		* The copy constructor is disabled for this class
		*/
		TSQueue(const TSQueue<T>&) = delete;
		/*
		* Clear this TSQueue's memory and delete it
		*/
		virtual ~TSQueue() { clear(); }
		/*
		* Delete all items from this queue's memory
		*/
		void clear() {
			std::scoped_lock(m_mtx);
			m_DEQueue.clear();
		}
		/*
		* Get the number of items in this queue
		*/
		size_t size() {
			std::scoped_lock(m_mtx);
			return m_DEQueue.size();
		}
		/*
		* Retrieve an element from the queue
		* 
		* @param index: The index of the element in the queue
		* @return A reference to the element of this queue at the given index
		*/
		T& at(unsigned int index) {
			std::scoped_lock(m_mtx);
			return m_DEQueue.at(index);
		}
		/*
		* Test whether this queue is empty
		*/
		bool empty() {
			std::scoped_lock(m_mtx);
			return m_DEQueue.empty();
		}
		/*
		* Get the item at the front of this queue
		* 
		* @return A reference to the item at the front of the double-ended queue
		*/
		const T& front() {
			std::scoped_lock(m_mtx);
			return m_DEQueue.front();
		}
		/*
		* Get the item at the front of this queue and remove it
		* 
		* @return The item at the front of this queue by value
		*/
		T popFront() {
			std::scoped_lock(m_mtx);
			T t = std::move(m_DEQueue.front());
			m_DEQueue.pop_front();
			return t;
		}
		/*
		* Get the item at the back of this queue
		*
		* @return A reference to the item at the back of the double-ended queue
		*/
		const T& back() {
			std::scoped_lock(m_mtx);
			return m_DEQueue.back();
		}
		/*
		* Get the item at the back of this queue and remove it
		*
		* @return The item at the back of this queue by value
		*/
		T popBack() {
			std::scoped_lock(m_mtx);
			T t = std::move(m_DEQueue.back());
			m_DEQueue.pop_back();
			return t;
		}
		/*
		* Add an item to the front of this queue
		* 
		* @param t: A reference to the item to push onto the front of the queue
		*/
		void pushFront(const T& t) {
			std::scoped_lock(m_mtx);
			m_DEQueue.emplace_front(std::move(t));
		}
		/*
		* Add an item to the back of this queue
		* 
		* @param t: A reference to the item to push onto the back of the queue
		*/
		void pushBack(const T& t) {
			std::scoped_lock(m_mtx);
			m_DEQueue.emplace_back(std::move(t));
		}

	private:
		// The double-ended queue wrapped by this TSQueue
		std::deque<T> m_DEQueue;
		// The mutex used to lock this queue for thread-safety
		std::mutex m_mtx;
	};

	/*
	* The interface which connects NetServer's and NetClient's to the internet
	* via the ASIO library
	*/
	class NetConnection : public std::enable_shared_from_this<NetConnection> {
	public:
		/*
		* Construct a new NetConnection with an ASIO context and a socket
		* 
		* @param context: The ASIO context for this connection to use
		* @param socket: The socket for this connection to use to connect to
		* the internet
		*/
		NetConnection(asio::io_context& context, asio::ip::tcp::socket& socket)
			: m_context(context), m_socket(std::move(socket)) {}
		/*
		* Initialize this connection with a context and a place to send incoming
		* messages (for NetClient's)
		* 
		* @param messagesIn: A pointer to a TSQueue of NetMessage's for this
		* connection to push back incoming messages to
		* @param maxMessageSize: The maximum message body size which can be
		* received by this connection in bytes (1024 by default)
		*/
		void init(TSQueue<NetMessage>* messagesIn,
			unsigned int maxMessageSize = 1024);
		/*
		* Attach this connection to a remote server (for NetClient's only)
		* 
		* @param endpoints: The results of an ASIO resolver used to generate
		* the IP of the remote server
		*/
		void connectToServer(
			const asio::ip::tcp::resolver::results_type& endpoints);
		/*
		* Attach this connection to a remote client (for NetServer's only)
		* 
		* @param ID: The unique ID number for this connection for identification
		* by the server
		*/
		void connectToClient(unsigned int ID);
		/*
		* Test whether this connection is currently open
		*/
		bool isConnected();
		/*
		* Get the ID number of this connection (for NetServer's only)
		*/
		unsigned int getID() const { return m_ID; }
		/*
		* Set the maximum message body size which can be received by this
		* connection in bytes
		*/
		void setMaxMessageSize(unsigned int maxMessageSize) {
			m_maxMessageSize = maxMessageSize;
		}
		/*
		* Send a message over this connection to the remote machine
		* 
		* @param message: The message to send over this connection
		*/
		void send(const NetMessage& message);
		/*
		* Detach this connection from whatever remote machine it is connected to
		*/
		void disconnect();
		/*
		* Free this connection's memory and disconnect it
		*/
		void destroy();

	private:
		// Whether this connection is connected to a remote machine
		bool m_connected = false;
		// The ASIO context to perform networking within
		asio::io_context& m_context;
		// The ASIO socket to read and write data with
		asio::ip::tcp::socket m_socket;
		// The ID number of this connection (for NetServer's only)
		unsigned int m_ID = 0;
		// A pointer to the queue to push incoming messages to
		TSQueue<NetMessage>* m_messagesIn = nullptr;
		// The queue of messages waiting to be sent asynchronously with ASIO
		TSQueue<NetMessage> m_messagesOut;
		// Temporary message to be populated with data by readHeader() and
		// readBody()
		NetMessage m_tempMessage;
		// The maximum message body size in bytes this connection will accept
		unsigned int m_maxMessageSize = 1024;

		// Read in a message header from the network asynchronously
		void readHeader();
		// Read in a message body from the network asynchronously
		void readBody();
		// Add a message to messages in queue
		void addMessageIn();
		// Write a message header from the messages out queue
		void writeHeader();
		// Write a message body from the messages out queue
		void writeBody();
	};

	/*
	A client which can connect to NetServer's over a connection using the ASIO
	library to access the internet
	*/
	class NetClient {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		NetClient(NetClient& nc) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static NetClient& getInstance() {
			return m_instance;
		}

		/*
		* Initialize this client's network connection
		* 
		* @param maxMessageSize: The maximum size in bytes of any message to be
		* received by a server to this client (1024 by default)
		*/
		void init(unsigned int maxMessageSize = 1024);
		/*
		* Connect this network client to a remote server at the given address
		* and port
		* 
		* @param address: The IP address of the server to connect to
		* @param port: The port number to attempt to connect to the server on
		*/
		void connect(const std::string& address, unsigned int port);
		/*
		* Test whether this client is connected to a server
		*/
		bool isConnected() const { return m_connection->isConnected(); }
		/*
		* Set the max message size for this client's connection in bytes
		*/
		void setMaxMessageSize(unsigned int maxMessageSize) {
			m_connection->setMaxMessageSize(maxMessageSize);
		}
		/*
		* Send a message to the server this network client is connected to
		* 
		* @param message: The NetMessage to send to the server
		*/
		void send(const NetMessage& message);
		/*
		* Get a reference to the queue of messages coming into this client from
		* the server
		*/
		TSQueue<NetMessage>& getMessagesIn() { return m_messagesIn; }
		/*
		* Disconnect this client from a server if it is connected
		*/
		void disconnect();
		/*
		* Disconnect this client and free it's memory
		*/
		void destroy();

	private:
		// The singleton instance of this class
		static NetClient m_instance;
		/*
		* The constructor is disabled for this class
		*/
		NetClient() : m_socket(m_context) {}

		// The ASIO context to do networking with
		asio::io_context m_context;
		// The thread for the ASIO context to do work in
		std::thread m_ASIOThread;
		// The socket to pass to the connection
		asio::ip::tcp::socket m_socket;
		// The connection used to access the internet
		std::shared_ptr<NetConnection> m_connection;
		// The queue for the connection to push messages from the server into
		// the back of
		TSQueue<NetMessage> m_messagesIn;
	};

	/*
	* A server which can manager connections over the internet from multiple
	* NetClient's using the ASIO library
	*/
	class NetServer {
	public:
		/*
		* Construct a new network server to manage many connections
		* 
		* @param port: The port number to listen for new network connections on
		*/
		NetServer(unsigned short port) : m_acceptor(m_context,
			asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}
		/*
		* Start listening for new clients connecting to this server
		* 
		* @param maxMessageSize: The maximum message body size which can be
		* received by a client in bytes (1024 by default)
		* @return Whether the server could be started
		*/
		bool init(unsigned int maxMessageSize = 1024);
		/*
		* Test whether this server is still listening for new connections
		*/
		bool isActive() const { return m_acceptor.is_open(); }
		/*
		* Send a message to the given client over its connection
		* 
		* @param client: A pointer to the client to send the message to
		* @param message: A reference to the message to send
		* @return Whether the message could be sent
		*/
		bool messageClient(std::shared_ptr<NetConnection> client,
			const NetMessage& message);
		/*
		* Send a message to all clients except, optionally, for one client
		* 
		* @param message: The message to broadcast
		* @param ignoredClient: A pointer to a client to ignore when sending
		* the message (nullptr by default)
		*/
		void messageAllClients(const NetMessage& message,
			std::shared_ptr<NetConnection> ignoredClient = nullptr);
		/*
		* Update this server and call onMessage for all new messages since the
		* last update
		* 
		* @param maxMessages: The maximum number of messages which can be passed
		* to the onMessage function this update (-1 for +inf)
		*/
		void update(int maxMessages);
		/*
		* Set the maxmimum message size for the clients' connections
		*/
		void setMaxMessageSize(unsigned int maxMessageSize);
		/*
		* Stop listening for new connections, close all existing connections and
		* free this NetServer's memory
		*/
		void destroy();
		
	protected:
		// The queue of messages in from the clients connected to this server
		TSQueue<NetMessage> m_messagesIn;
		// The logging system for this server
		LogManager& m_log = LogManager::getInstance();

		/*
		* A client has made a connection to this server
		* 
		* @param client: A pointer to the new connection made by this client
		* @return Whether to accept the connection to the server
		*/
		virtual bool onConnect(std::shared_ptr<NetConnection> client) {
			return true;
		}
		/*
		* A client has sent a NetMessage to this server
		* 
		* @param message: The message the client send, containing a pointer to
		* the NetConnection which owns it
		*/
		virtual void onMessage(const NetMessage& message) {}
		/*
		* A client has been disconnected from the server
		* 
		* @param client: A pointer to the client which is disconnecting
		*/
		virtual void onDisconnect(std::shared_ptr<NetConnection> client) {}

	private:
		// The currently active and valid clients connected to this server
		std::deque<std::shared_ptr<NetConnection>> m_clients;
		// The ASIO context for this server
		asio::io_context m_context;
		// The thread to let the ASIO context work in
		std::thread m_ASIOThread;
		// The ASIO port to let new connections in on
		asio::ip::tcp::acceptor m_acceptor;
		// The current working ID of a new connection
		unsigned int m_currentID = 100;
		// The maximum size of any messages to receive from clients
		unsigned int m_maxMessageSize = 1024;

		/*
		* Wait for a new client to connect to this server and generate a new
		* NetConnection when one is found
		*/
		void waitForConnection();
	};
}

#endif