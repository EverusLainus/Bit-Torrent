CXX = clang++
flags := -std=c++17 
include_header := -I./cpp-bencoding/include/ -I./lib_header/ -I./lib_header/crypto/ -I./cpr/include/cpr -I./lib_header/crypto
include_library := -L./cpp-bencoding/build/src -L./lib_binary -L./cpr/build/lib
library := -lbencoding -lsha1 -lcpr -lz -lcpr.1 -W1,-rpath,./cpr/build/lib -lcurl.4

all: main

main: main.cpp ParseTorrentFile.o RequestTracker.o PeerConnection.o BitTorrentMessage.o Connect.o
	$(CXX) main.cpp ParseTorrentFile.o RequestTracker.o PeerConnection.o connect.o BitTorrentMessage.o -o main $(flags) $(include_header) $(include_library) $(library)

ParseTorrentFile.o: ParseTorrentFile.cpp 
	$(CXX) -c  ParseTorrentFile.cpp -o ParseTorrentFile.o $(flags) $(include_header) $(include_library) 

RequestTracker.o: RequestTracker.cpp
	$(CXX) -c RequestTracker.cpp -o RequestTracker.o $(flags) $(include_header) $(include_library) 

PeerConnection.o: PeerConnection.cpp
	$(CXX) -c PeerConnection.cpp -o PeerConnection.o $(flags) $(include_header) $(include_library) 

Connect.o: Connect.cpp
	$(CXX) -c Connect.cpp -o Connect.o $(flags) $(include_header) $(include_library) 

BitTorrentMessage.o: BitTorrentMessage.cpp
	$(CXX) -c BitTorrentMessage.cpp -o BitTorrentMessage.o $(flags) $(include_header) $(include_library) 
clean:
	rm -f Connect.o PeerConnection.o RequestTracker.o ParseTorrentFile.o main

# export DYLD_LIBRARY_PATH=./cpr/build/lib:$DYLD_LIBRARY_PATH
# ./main -s ./files/MoralPsychHandbook.torrent -d ./files