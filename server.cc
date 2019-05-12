#include "server.h"

namespace firesse {

Server::Server(int port, const std::function<void(std::unique_ptr<Stream>)>& callback)
		: callback_(callback),
		  firecgi_server_(port,
		                  [this](firecgi::Request* request) { OnRequest(request); },
						  1) {}

void Server::Serve() {
	firecgi_server_.Serve();
}

void Server::OnRequest(firecgi::Request* request) {
	request->WriteHeader("Content-Type", "text/event-stream; charset=utf-8");
	request->WriteHeader("Cache-Control", "no-cache");
	request->WriteHeader("X-Accel-Buffering", "no");
	request->WriteBody("");
	callback_(std::make_unique<Stream>(request));
}

} // namespace firesse
