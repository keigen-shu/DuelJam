//  Models/NoteAlgorithm.hpp :: Note object algorithms
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_NOTE_ALGORITHM_H
#define MODEL_NOTE_ALGORITHM_H

#include <map>          // std::map
#include <type_traits>  // std::enable_if, std::is_base_of, std::is_same

#include "Note.hpp"

// TODO C++14 : Use std::enable_if_t on all the templates below

template< typename NoteType >
	auto cmpNote_Greater    (const NoteType &a, const NoteType &b)
	->  typename std::enable_if <
			std::is_base_of< Note, NoteType >::value,
			bool >::type
{ return (a .t == b .t) ? (a .k <  b .k) : (a .t <  b .t); }

template< typename NoteType >
	auto cmpNote_GreaterTime(const NoteType &a, const NoteType &b)
	->  typename std::enable_if <
			std::is_base_of< Note, NoteType >::value,
			bool >::type
{ return (a .t <  b .t); }


template< typename Container, typename NoteType = typename Container::value_type >
	auto splitNotes_byKey   (const Container &container)
	-> typename std::map <
			ENoteKey,
			typename std::enable_if <
				std::is_pointer< NoteType >::value == false &&
				std::is_base_of< EventNote, NoteType >::value,
				Container >::type
			>   // For EventNote reference containers
{
	std::map< ENoteKey, Container > note_map;
	for(auto it = std::begin(container); it != std::end(container); it++)
		note_map[(*it).k].push_back(*it);

	return note_map;
}

template< typename Container, typename NoteType = typename Container::value_type >
	auto splitNotes_byKey   (const Container &container)
	-> typename std::map <
			ENoteKey,
			typename std::enable_if <
				std::is_pointer< NoteType >::value == true  &&
				std::is_same   < typename Container::const_reference, EventNote * const & >::value,
				Container >::type
			>   // For EventNote pointer containers
{
	std::map< ENoteKey, Container > note_map;
	for(auto it = std::begin(container); it != std::end(container); it++)
		note_map[(*it)->k].push_back(*it);

	return note_map;
}

inline bool cmpobjNote_Greater    (const Note * const &a, const Note * const &b)
{ return (a->t == b->t) ? (a->k <  b->k) : (a->t <  b->t); }

inline bool cmpobjNote_GreaterTime(const Note * const &a, const Note * const &b)
{ return (a->t <  b->t); }

#endif
