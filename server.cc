#include "server.h"

namespace firesse {

Server::Server(int port, const std::function<void(Stream*)>& callback)
		: callback_(callback),
		  firecgi_server_(port,
		                  [this](firecgi::Request* request) { OnRequest(request); },
						  1) {}

void Server::Serve() {
	firecgi_server_.Serve();
}

void Server::Shutdown() {
	firecgi_server_.Shutdown();
}

void Server::RegisterSignalHandlers() {
	firecgi_server_.RegisterSignalHandlers();
}

void Server::OnRequest(firecgi::Request* request) {
	request->WriteHeader("Content-Type", "text/event-stream; charset=utf-8");
	request->WriteHeader("Cache-Control", "no-cache");
	request->WriteHeader("X-Accel-Buffering", "no");
	request->WriteBody("");
	auto stream = new Stream(request);
	callback_(stream);
}

} // namespace firesse
