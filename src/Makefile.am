SUBDIRS = libawe kiss_fft130

bin_PROGRAMS = DuelJam

DuelJam_CXXFLAGS = $(ClanLib_CFLAGS)
DuelJam_LDADD = libawe/libawe.a kiss_fft130/libkiss_fft.a
DuelJam_LDFLAGS = $(ClanLib_LIBS)
DuelJam_SOURCES = \
	Models/ChronoTClock.cpp         \
	Models/ChronoTTime.cpp          \
	Models/EventNoteInstance.cpp    \
	Models/Measure.cpp              \
	Models/Sequence.cpp             \
	Models/Tracker.cpp              \
	\
	Scenes/MusicSelector.cpp    \
	\
	UI/FFT.cpp              \
	UI/Graph.cpp            \
	UI/Graph_FrameRate.cpp  \
	UI/Graph_Time.cpp       \
	UI/Slider.cpp           \
	UI/SwitchButton.cpp     \
	\
	clanAlt_Font.cpp        \
	clanExt_JSONFile.cpp    \
	clanExt_JSONReader.cpp  \
	\
	AudioManager.cpp \
	AudioTrack.cpp \
	AudioVoice.cpp \
	Chart_O2Jam.cpp \
	Chart_BMS.cpp \
	Music.cpp \
	MusicScanner.cpp \
	InputManager.cpp \
	Game.cpp \
	Main.cpp
