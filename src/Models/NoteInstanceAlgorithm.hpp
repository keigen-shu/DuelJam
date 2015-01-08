//  Models/NoteInstanceAlgorithm.hpp :: Note instance object algorithms
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#pragma once

#include <future>
#include "NoteAlgorithm.hpp"

using NEPair = std::pair< Measure::NSs, Measure::NLs >;

/*! Connects two lists of notes to a list of long notes.
 *
 * This function assumes that all notes being sent into this function
 * have the same key.
 *
 * This function will attempt to fix erroneous long note sequences.
 * The fixes are based on the following conditions:
 * - A HOLD note MUST be located before a RELEASE note.
 * - Both ends of the long notes MUST have identical Sample IDs, except
 *   in the case of BMS's LN_TYPE 1 mode.
 * - Switching between HOLD notes and NORMAL notes are allowed because
 *   they both make sounds.
 * - Deletion of RELEASE notes are allowed because they do not make any sound.
 */
NEPair zip_toNL(Measure::NSs &H, Measure::NSs &R)
{
	Measure::NSs S;
	Measure::NLs L;

	while(!H.empty())
	{
		if (R.empty())
		{
			fprintf(stderr, "Note [debug] Converting lone HOLD to NORMAL.\n");
			Measure::NSs::iterator h = H.begin();
			S.emplace_back(*h, h->getAudio());
			H.erase(H.begin());
		} else {
			Measure::NSs::iterator h = H.begin(), r = R.begin();

			////    CHECK AND FIX NOTES    ////////////////////////////
			// TODO Put note checking logic into a class.
			bool errTime = h->t                   >  r->t;
			bool errSmpl = h->getAudio().sampleID != r->getAudio().sampleID
			            && r->getAudio().sampleID != 0;

			if (h->t == r->t) {
				fprintf(stderr, "Note [debug] Bad long note: Instant RELEASE.\n");
				fprintf(stderr, "     [---->] Deleting RELEASE.\n");

				if (errSmpl)
					fprintf(stderr, "     [---->] Note: It has mis-matched samples.\n");

				R.erase(r);
				continue;
			}

			if (errTime)
			{
				fprintf(stderr, "Note [debug] Bad long note: Invalid time.\n");
				fprintf(stderr, "     [---->] Deleting RELEASE.\n");

				if (errSmpl)
					fprintf(stderr, "     [---->] Note: It has mis-matched samples.\n");

				// TODO If the note before `r` is a NORMAL note with the same sample ID,
				//      one may change the type of that NORMAL note to HOLD. But we don't
				//      have access to any NORMAL notes.
				R.erase(r);
				continue;
			}

			if (errSmpl)
			{
				fprintf(stderr, "Note [debug] Bad long note: Sound mismatch.\n");

				Measure::NSs::iterator t = h;
				if ((++t) != H.end())
				{
					if (t->t < r->t)
					{
						fprintf(stderr, "     [---->] Converting HOLD to NORMAL since the next hold note can fit.\n");
						S.emplace_back(*h, h->getAudio());
						H.erase(h);
						continue;
					}
				}

				fprintf(stderr, "     [---->] Deleting RELEASE.\n");
				R.erase(r);
				continue;
			}

			L.emplace_back(*h, *r);
			H.erase(h);
			R.erase(r);
		}
	}

	return { S, L };
}


NEPair zip_toNLs(const Measure::NSs &HNS, const Measure::NSs &RNS)
{
	std::map<ENoteKey, Measure::NSs> Hmap = splitNotes_byKey(HNS);
	std::map<ENoteKey, Measure::NSs> Rmap = splitNotes_byKey(RNS);

	std::vector< std::future< NEPair > > zippedFutures;

	for(auto node : Hmap)
	{
		zippedFutures.push_back(
				std::async(
					// std::launch::async, zip_toNL, Hmap[node.first], Rmap[node.first]
					[&]() { return zip_toNL(Hmap[node.first], Rmap[node.first]); }
					));
	}

	auto pair = std::make_pair<Measure::NSs, Measure::NLs>({}, {});

	while(!zippedFutures.empty())
	{
		auto it = zippedFutures.begin();
		auto result = it->get();
		pair.first .insert(pair.first .end(), result.first .begin(), result.first .end());
		pair.second.insert(pair.second.end(), result.second.begin(), result.second.end());
		zippedFutures.erase(it);
	}

	return pair;
}
