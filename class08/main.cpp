#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>

using namespace std;
using namespace boost::program_options;
using namespace boost::program_options::command_line_style;


int  sockLocalNum = 0;
int sockRemoteNum = 0;
string resultFile = "";
string dataFile   = "";

class Connect {
public:
	static int client(bool isUDP) {
		char message[] = "Hello there!\n";
		char buf[sizeof(message)];

		int sock = -1;
		if (!isUDP) {
			sock = socket(AF_INET, SOCK_STREAM, 0);
		} else {
			sock = socket(AF_INET, SOCK_DGRAM , 0);
		}
		if (sock < 0) {
			cerr << "Problem with socket creation!" << endl;
			return 1;
		}

		if (sockLocalNum != 0) {
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(sockLocalNum);
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			bind(sock, (struct sockaddr *)&addr, sizeof(addr));
		}
		

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(sockRemoteNum);
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			cerr << "Problem with connection establishment!" << endl;
			return 1;
		}

		send(sock, message, sizeof(message), 0);
		recv(sock, buf, sizeof(message), 0);

		cout << buf << endl;
		close(sock);

		return 0;
	}

protected:
	static void reverse(char* arr, int size) {
		if (size <= 1) { return; }
		for (int i = 0; i < size / 2; i++) {
			char tmp = arr[i];
			arr[i] = arr[size - i - 1];
			arr[size - i - 1] = tmp;
		}
	}
};

class TCP: public Connect {
public:
	static int server() {
		cout << "TCP server\n---" << endl;

		char buf[4 * 1024 * 1024];
		int listener = socket(AF_INET, SOCK_STREAM, 0);			
		if (listener < 0) {
			cerr << "Problem with socket creation!" << endl;
			return 1;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(sockLocalNum);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			cerr << "Problem with bind!" << endl;
			return 1;
		}

		listen(listener, 1);

		while (1) {
			int sock = accept(listener, NULL, NULL);
			if (sock < 0) {
				cerr << "Problem with accept!" << endl;
				return 1;
			}

			while (1) {
				int bytes_read = recv(sock, buf, 1024, 0);
				if (bytes_read <= 0) break;
				reverse(buf, bytes_read - 1);
				send(sock, buf, bytes_read, 0);
			}

			close(sock);
		}
		return 0;
	}
};

class UDP: public Connect {
public:
	static int server() {
		cout << "UDP server\n---" << endl;

		int bufSize = 401;
		char buf[bufSize];

		int listener = socket(AF_INET, SOCK_DGRAM, 0);			
		if (listener < 0) {
			cerr << "Problem with socket creation!" << endl;
			return 1;
		}
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(sockLocalNum);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			cerr << "Problem with bind!" << endl;
			return 1;
		}

		while (1) {
			int bytes_read = recvfrom(listener, buf, bufSize, 0, NULL, NULL);
			if (bytes_read > 0) {
				reverse(buf, bytes_read - 1);
			}
			send();
		}
	}
};

int main(int argc, char** argv) {
	options_description desc("Allowed options");
	desc.add_options()
		("help"  , "produce help message")
		("udp"   , "use UDP as a transport")
		("tcp"   , "use TCP as a transport")
		("server", "start as a server")
		("client", "start as a client")
		("local" , value<int>(& sockLocalNum), "set a local port number")
		("remote", value<int>(&sockRemoteNum), "set a remote port number")
		("data"  , value<string>(&  dataFile), "set a file name with data to send")
		("result", value<string>(&resultFile), "set a file name write result to")
	;

	variables_map vm;
	store(
		command_line_parser(argc, argv).options(desc)
		.style(command_line_style::default_style
			|  command_line_style::case_insensitive
			|  command_line_style::allow_slash_for_short
			|  command_line_style::allow_long_disguise)
		.run()
    , vm);
	notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 0;
	}

	if (vm.count("server")) {
		if (vm.count("udp")) {
			return UDP::server();
		}
		return TCP::server();
	}

	if (vm.count("client")) {
		return TCP::client(vm.count("udp"));
	}

	cout << "Not client or server!" << endl;
	return 1;
}