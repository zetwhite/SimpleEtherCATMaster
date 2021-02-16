CC = g++
TARGET = EcatMaster

$(TARGET) : main.o ethernetHelper.o ethercatHeader.o util.o packet.o
	$(CC) -o $@ $^

main.o : main.cpp
	$(CC) -c -o $@ $^

ethernetHelper.o : layer2/ethernetHelper.cpp 
	$(CC) -c -o $@ $^

ethercatHeader.o : layer3/ethercatHeader.cpp
	$(CC) -c -o $@ $^

util.o : util/util.cpp
	$(CC) -c -o $@ $^

packet.o : util/packet.cpp 
	$(CC) -c -o $@ $^

clean : rm *.o