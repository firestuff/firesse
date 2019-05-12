#pragma once

#include <set>

#include "firecgi/server.h"

#include "index.h"
#include "keepalive.h"
#include "stream.h"

namespace firesse {

class Server {
  public:
	Server(int port, const std::function<void(Stream*)>& callback);
	void Serve();
	void Shutdown();
	void RegisterSignalHandlers();

  private:
	void OnRequest(firecgi::Request* request);

	std::function<void(Stream*)> callback_;
	Index index_;
	KeepAlive keep_alive_;
	firecgi::Server firecgi_server_;
};

} // namespace firesse
