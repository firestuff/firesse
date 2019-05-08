#include "firesse.h"

namespace firesse {

Server::Server(int port, const std::function<void(std::unique_ptr<Stream>)>& callback)
		: callback_(callback),
		  firecgi_server_(port,
		                  [this](firecgi::Request* request) { OnRequest(request); },
						  1,
						  {"HTTP_ACCEPT"}) {}

void Server::Serve() {
	firecgi_server_.Serve();
}

void Server::OnRequest(firecgi::Request* request) {
	if (request->GetParam("HTTP_ACCEPT") != "text/event-stream") {
		LOG(WARNING) << "bad HTTP_ACCEPT: " << request->GetParam("HTTP_ACCEPT");
		request->WriteHeader("Status", "400 Bad Request");
		request->WriteHeader("Content-Type", "text-plain");
		request->WriteBody("No \"Accept: text/event-stream\" header found in request. Please call this endpoint using EventSource.");
		request->End();
		return;
	}
	request->WriteHeader("Content-Type", "text/event-stream");
	request->WriteBody("");
	callback_(std::make_unique<Stream>(request));
}

} // namespace firesse
