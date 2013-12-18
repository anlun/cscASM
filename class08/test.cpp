class UDP: public Connect {
public:
	static int client() {
		int sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) {
			cerr << "Problem with socket creation!" << endl;
			return 1;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(sockRemoteNum);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		
		connect(sock, (struct sockaddr *)&addr, sizeof(addr));
		send(sock, message, sizeof(message), 0);

		close(sock);
	}


// private:
};