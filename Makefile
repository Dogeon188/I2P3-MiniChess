CXX = g++
CXXFLAGS = --std=c++2a -Wall -Wextra -O3 -g

SOURCES_DIR = src
UNITTEST_DIR = unittest

BUILD_DIR = build
SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)

MAIN = $(SOURCES_DIR)/main.cpp
PLAYERS = $(wildcard $(SOURCES_DIR)/player/*.cpp)
UNITTESTS = $(wildcard $(UNITTEST_DIR)/*.cpp)
STATE_SOURCE = $(SOURCES_DIR)/state/state.cpp
POLICY_DIR = $(SOURCES_DIR)/policy

TARGET_PLAYER = $(PLAYERS:$(SOURCES_DIR)/player/%.cpp=%)
TARGET_PLAYER_MIXED = mixed
TARGET_SUBMISSION = submission
TARGET_MAIN = main
TARGET_OTHER = selfplay benchmark
TARGET_UNITTEST = $(UNITTESTS:$(UNITTEST_DIR)/%.cpp=%)
OTHER = action state gamelog.txt


.PHONY: all clean
all: |$(BUILD_DIR) $(TARGET_MAIN) $(TARGET_PLAYER) $(TARGET_UNITTEST)
player: |$(BUILD_DIR) $(TARGET_PLAYER)

# make build dir if it doesn't exist
$(BUILD_DIR):
	mkdir "$(BUILD_DIR)"
	mkdir "$(UNITTEST_DIR)/build"

# build target
ifeq ($(OS), Windows_NT)
$(TARGET_PLAYER): % : $(SOURCES_DIR)/player/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/player/$@.exe $(STATE_SOURCE) $(POLICY_DIR)/$@.cpp $< 
$(TARGET_PLAYER_MIXED): % : $(SOURCES_DIR)/player/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/player/%.exe $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $<
$(TARGET_SUBMISSION):
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/submission.exe $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $(SOURCES_DIR)/player/abprune_pss.cpp
$(TARGET_MAIN): % : $(SOURCES_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@.exe $< 
$(TARGET_OTHER): %: $(SOURCES_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@.exe $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $<
$(TARGET_UNITTEST): %: $(UNITTEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $(UNITTEST_DIR)/build/$@.exe $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $<
else
$(TARGET_PLAYER): % : $(SOURCES_DIR)/player/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/player/$@ $(STATE_SOURCE) $(POLICY_DIR)/$@.cpp $< 
$(TARGET_PLAYER_MIXED): % : $(SOURCES_DIR)/player/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/player/$@ $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $<
$(TARGET_SUBMISSION):
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/submission $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $(SOURCES_DIR)/player/abprune_hce.cpp
$(TARGET_MAIN): % : $(SOURCES_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@ $< 
$(TARGET_OTHER): %: $(SOURCES_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@ $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $<
$(TARGET_UNITTEST): %: $(UNITTEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $(UNITTEST_DIR)/build/$@ $(STATE_SOURCE) $(POLICY_DIR)/*.cpp $<
endif
