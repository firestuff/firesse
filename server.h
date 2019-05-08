#pragma once

#include "firecgi/server.h"
#include "stream.h"

namespace firesse {

class Server {
  public:
	Server(int port, const std::function<void(std::unique_ptr<Stream>)>& callback);
	void Serve();

  private:
	void OnRequest(firecgi::Request* request);

	std::function<void(std::unique_ptr<Stream>)> callback_;
	firecgi::Server firecgi_server_;
};

} // namespace firesse