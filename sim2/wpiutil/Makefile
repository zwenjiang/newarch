TOPDIR=../..

include $(TOPDIR)/makefiles/defaults.mk

SOURCES = \
	./cpp/Base64.cpp\
	./cpp/EventLoopRunner.cpp\
	./cpp/future.cpp\
	./cpp/hostname.cpp\
	./cpp/HttpParser.cpp\
	./cpp/HttpServerConnection.cpp\
	./cpp/HttpUtil.cpp\
	./cpp/http_parser.cpp\
	./cpp/json.cpp\
	./cpp/json_binary_reader.cpp\
	./cpp/json_binary_writer.cpp\
	./cpp/json_parser.cpp\
	./cpp/json_pointer.cpp\
	./cpp/json_serializer.cpp\
	./cpp/leb128.cpp\
	./cpp/llvm/ConvertUTF.cpp\
	./cpp/llvm/ConvertUTFWrapper.cpp\
	./cpp/llvm/Error.cpp\
	./cpp/llvm/ErrorHandling.cpp\
	./cpp/llvm/Hashing.cpp\
	./cpp/llvm/ManagedStatic.cpp\
	./cpp/llvm/NativeFormatting.cpp\
	./cpp/llvm/Path.cpp\
	./cpp/llvm/raw_ostream.cpp\
	./cpp/llvm/raw_os_ostream.cpp\
	./cpp/llvm/SmallPtrSet.cpp\
	./cpp/llvm/SmallVector.cpp\
	./cpp/llvm/StringExtras.cpp\
	./cpp/llvm/StringMap.cpp\
	./cpp/llvm/StringRef.cpp\
	./cpp/llvm/Twine.cpp\
	./cpp/PortForwarder.cpp\
	./cpp/raw_istream.cpp\
	./cpp/raw_socket_istream.cpp\
	./cpp/raw_socket_ostream.cpp\
	./cpp/raw_uv_ostream.cpp\
	./cpp/SafeThread.cpp\
	./cpp/sha1.cpp\
	./cpp/SocketError.cpp\
	./cpp/TCPAcceptor.cpp\
	./cpp/TCPConnector.cpp\
	./cpp/TCPConnector_parallel.cpp\
	./cpp/TCPStream.cpp\
	./cpp/timestamp.cpp\
	./cpp/UDPClient.cpp\
	./cpp/uv/Async.cpp\
	./cpp/uv/Check.cpp\
	./cpp/uv/FsEvent.cpp\
	./cpp/uv/GetAddrInfo.cpp\
	./cpp/uv/GetNameInfo.cpp\
	./cpp/uv/Handle.cpp\
	./cpp/uv/Idle.cpp\
	./cpp/uv/Loop.cpp\
	./cpp/uv/NameToAddr.cpp\
	./cpp/uv/NetworkStream.cpp\
	./cpp/uv/Pipe.cpp\
	./cpp/uv/Poll.cpp\
	./cpp/uv/Prepare.cpp\
	./cpp/uv/Process.cpp\
	./cpp/uv/Signal.cpp\
	./cpp/uv/Stream.cpp\
	./cpp/uv/Tcp.cpp\
	./cpp/uv/Timer.cpp\
	./cpp/uv/Tty.cpp\
	./cpp/uv/Udp.cpp\
	./cpp/uv/Work.cpp\
	./cpp/WebSocket.cpp\
	./cpp/WebSocketServer.cpp\
	./libuv/src/fs-poll.cpp\
	./libuv/src/idna.cpp\
	./libuv/src/inet.cpp\
	./libuv/src/strscpy.cpp\
	./libuv/src/threadpool.cpp\
	./libuv/src/timer.cpp\
	./libuv/src/unix/async.cpp\
	./libuv/src/unix/core.cpp\
	./libuv/src/unix/dl.cpp\
	./libuv/src/unix/fs.cpp\
	./libuv/src/unix/fsevents.cpp\
	./libuv/src/unix/getaddrinfo.cpp\
	./libuv/src/unix/getnameinfo.cpp\
	./libuv/src/unix/loop-watcher.cpp\
	./libuv/src/unix/loop.cpp\
	./libuv/src/unix/no-fsevents.cpp\
	./libuv/src/unix/no-proctitle.cpp\
	./libuv/src/unix/pipe.cpp\
	./libuv/src/unix/poll.cpp\
	./libuv/src/unix/posix-hrtime.cpp\
	./libuv/src/unix/process.cpp\
	./libuv/src/unix/procfs-exepath.cpp\
	./libuv/src/unix/proctitle.cpp\
	./libuv/src/unix/pthread-fixes.cpp\
	./libuv/src/unix/signal.cpp\
	./libuv/src/unix/stream.cpp\
	./libuv/src/unix/tcp.cpp\
	./libuv/src/unix/thread.cpp\
	./libuv/src/unix/udp.cpp\
	./libuv/src/uv-common.cpp\
	./libuv/src/uv-data-getter-setters.cpp\
	./libuv/src/version.cpp\
	./unix/Demangle.cpp\
	./cygwin/StackTrace.cpp
	
ifeq ($(MYOS),Cygwin)
  SOURCES += 	./libuv/src/unix/cygwin.cpp\
	            ./libuv/src/unix/sysinfo-loadavg.cpp
endif
ifeq ($(MYOS),Darwin)
  SOURCES += 	./libuv/src/unix/darwin.cpp\
  		        ./libuv/src/unix/darwin-proctitle.cpp
endif
ifeq ($(MYOS),Linux)
  SOURCES += 	./libuv/src/unix/linux-core.cpp\
  		        ./libuv/src/unix/linux-inotify.cpp\
		        ./libuv/src/unix/linux-syscalls.cpp\
	            ./libuv/src/unix/sysinfo-loadavg.cpp
endif

TARGET=wpiutil

LOCAL_CFLAGS += -Iinclude -Ilibuv/include -Ilibuv/src

include $(TOPDIR)/makefiles/buildlib.mk
