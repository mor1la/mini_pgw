# Компилятор
CXX := g++
# Флаги компиляции
CXXFLAGS := -std=c++11 -Wall -Wextra -I. -IConfigLoader
# Флаги линковки
LDFLAGS := 
# Имя исполняемого файла
TARGET := mini_p_server

# Исходные файлы (с указанием путей)
SRCS := main.cpp UdpServer.cpp \
        ConfigLoader/ClientConfigLoader.cpp \
        ConfigLoader/ServerConfigLo# Исходные файлы (с указанием путей)
SRCS := main.cpp UdpServer.cpp \
        ConfigLoader/ClientConfigLoader.cpp \
        ConfigLoader/ServerConfigLoader.cpp

# Объектные файлы (с путями, чтобы избежать конфликтов)
OBJS := $(SRCS:.cpp=.o)

# Заголовочные файлы
HEADERS := ConfigLoader/ClientConfigLoader.h \
           ConfigLoader/IConfigLoader.h \
           ConfigLoader/ServerConfigLoader.h \
           ConfigLoader/ClientSettings.h \
           ConfigLoader/ServerSettings.h \
           UdpServer.hader.cpp

# Объектные файлы (с путями, чтобы избежать конфликтов)
OBJS := $(SRCS:.cpp=.o)

# Заголовочные файлы
HEADERS := ConfigLoader/ClientConfigLoader.h \
           ConfigLoader/IConfigLoader.h \
           ConfigLoader/ServerConfigLoader.h \
           ConfigLoader/ClientSettings.h \
           ConfigLoader/ServerSettings.h \
           UdpServer.h

# Правило по умолчанию
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Правило для компиляции .cpp в .o (общее для всех файлов)
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка (удаляем все .o и исполняемый файл)
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean