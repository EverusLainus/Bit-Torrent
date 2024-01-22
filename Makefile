CXX = clang++
flags := -std=c++17 
include_header := -I./cpp-bencoding/include/ -I./lib_header/ -I./lib_header/crypto/ -I./cpr/include/cpr 
include_library := -L./cpp-bencoding/build/src -L./lib_binary -L./cpr/build/lib 
library := -lbencoding -lsha1 -lcpr -lz -lcpr.1 -W1,-rpath,./cpr/build/lib -lcurl.4 -lloguru

all: main

main: main.cpp ParseTorrentFile.o RequestTracker.o PeerConnection.o BitTorrentMessage.o Connect.o PieceManager.o Piece.o 
	$(CXX) main.cpp ParseTorrentFile.o RequestTracker.o PeerConnection.o connect.o BitTorrentMessage.o PieceManager.o Piece.o -o main $(flags) $(include_header) $(include_library) $(library)

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

Piece.o: Piece.cpp
	$(CXX) -c Piece.cpp -o Piece.o $(flags) $(include_header) $(include_library) 


PieceManager.o: PieceManager.cpp
	$(CXX) -c PieceManager.cpp -o PieceManager.o $(flags) $(include_header) $(include_library)
clean:
	rm -f Connect.o PeerConnection.o RequestTracker.o ParseTorrentFile.o main PieceManager.o Piece.o ./downloads/MoralPsychHandbook.pdf

# export DYLD_LIBRARY_PATH=./cpr/build/lib:$DYLD_LIBRARY_PATH
# ./main -s ./files/MoralPsychHandbook.torrent -d ./files

# ./main -s ./files/kali-linux-2023.4-installer-arm64.iso.torrent  -d ./files
# tracker invalid request

# ./main -s ./files/kali-linux-2023.4-hyperv-amd64.7z.torrent -d ./files
# tracker invalid request

# ./main -s ./files/ubuntu-23.10.1-desktop-amd64.iso.torrent  -d ./files -l ./files/example.log
# till send intersted

# /main -s ./files/ubuntu-22.04.3-desktop-amd64.iso.torrent  -d ./files -l ./files/example.log
# till send interest

# ./main -s ./files/kali-linux-2023.4-installer-amd64.iso.torrent  -d ./files -l ./files/example.log
# tracker: invalid Request

# ./main -s ./files/debian-10.10.0-amd64-netinst.iso.torrent  -d ./files -l ./files/example.log
# crate handshake message

# ./main -s ./files/ubuntu-12.04.5-alternate-amd64.iso.torrent  -d ./files -l ./files/example.log
# tracker error: Requested download is not authorized for use with this tracker

# ./main -s ./files/ComputerNetworks.torrent  -d ./files -l ./files/example.log
# error whle parsein: unexpected character