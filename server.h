#pragma once

#include <set>

#include "firecgi/server.h"
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
	firecgi::Server firecgi_server_;

	std::mutex mu_;
	std::set<Stream*, IsFresherStream> streams_;
};

} // namespace firesse
