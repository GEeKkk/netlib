CXXFLAGS = -O0 -g  -Wall -I ../../.. -pthread
LDFLAGS = -lpthread
BASE_SRC = ../../base/Logging.cc ../../base/LogStream.cc ../../base/CurrentThread.cc ../../base/Timestamp.cc

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(LIB_SRC) $(BASE_SRC) $(filter %.cpp,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core