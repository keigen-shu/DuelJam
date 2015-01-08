//  Chart_BMS.hpp :: BMS chart type class
//  Copyright 2011 - 2013 Keigen Shu

#ifndef CHART_BMS_H
#define CHART_BMS_H

#include "Models/Chart.hpp"

struct Music;
class Chart_BMS : public Chart
{
public:
    using BMSMsg = std::pair<std::string, std::string>;

private:
    std::map<uint, double> measure_ts_z;    // Measure Z Time Signatures
    std::map<uint, BMSMsg> messages;        // Line number -> Message map

    void init_sequence();

protected:
    std::string bms_fullpath;
    std::string bms_filename;
    std::string stage_file;

	std::string name;
	std::string charter;
	uchar level;
	double tempo;

    std::vector<std::string>    script;

    uchar   WAV_ID_LENGTH;
    uchar   BMP_ID_LENGTH;
    uchar   BPM_ID_LENGTH;
    uchar   STP_ID_LENGTH;

    std::map<uint, std::string> wavs;
    std::map<uint, double>      bpms;
    std::map<uint, uint>        stops;

    unsigned int    type;
    std::string     genre;

    unsigned int    rank;
    double          vol;

    unsigned int    measures;

public:
    Chart_BMS(const std::string &path);

    const std::string& getGenre() const { return genre; }
    unsigned int getRank () const { return rank;  }
    unsigned int getType () const { return type;  }

    virtual void load_cover_art();
    virtual void load_chart    ();
    virtual void load_samples  ();
};

Music* scan_BMS_directory(const std::string &path);

#endif
