CFLAGS = -std=c++17 -Wall -Werror
PFLAGS = -lpthread -lrt

CONNECTIONS = $(shell find ./connections -name '*.cpp')

all:
	@$(foreach conn,$(wildcard $(CONNECTIONS)),$(call build,$(conn:./connections/conn_%.cpp=%)))

define build
    echo $(1) ;
	g++ $(CFLAGS) ./host/* ./utils/* ./connections/*.h ./connections/conn_$(1).cpp -o host_$(1) $(PFLAGS) ;
	echo host_$(1) ;
	g++ $(CFLAGS) ./client/* ./utils/* ./connections/*.h ./connections/conn_$(1).cpp -o client_$(1) $(PFLAGS) ;
	echo client_$(1) ;
endef

clean:
	@rm -f host_* client_*