#include "Measure.hpp"
#include "NoteAlgorithm.hpp"

void Measure::setSignature(const TSignature &new_value)
{
	signature  = new_value;
	tick_count = getTickCount(signature);
}

void Measure::sort_elements()
{
	std::sort(mCCs.begin(), mCCs.end(), cmpEventCC_Greater);
	std::sort(mNSs.begin(), mNSs.end(), cmpNote_Greater<EventNoteSingle>);
	std::sort(mNLs.begin(), mNLs.end(), cmpNote_Greater<EventNoteLong>);
}

Measure::NEs Measure::getNoteEvents()
{
	NEs e;
	e.reserve(mNSs.size() + mNLs.size());

	for(NSs::size_type i = 0; i < mNSs.size(); i++)
		e.push_back(mNSs.data() + i);
	for(NLs::size_type i = 0; i < mNLs.size(); i++)
		e.push_back(mNLs.data() + i);

	std::sort(e.begin(), e.end(), cmpobjNote_Greater);

	return e;
}
