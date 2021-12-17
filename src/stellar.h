// ==========================================================================
//                    STELLAR - SwifT Exact LocaL AligneR
//                   http://www.seqan.de/projects/stellar/
// ==========================================================================
// Copyright (C) 2010-2012 by Birte Kehr
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your options) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ==========================================================================
// Author: Birte Kehr <birte.kehr@fu-berlin.de>
// ==========================================================================

#ifndef SEQAN_HEADER_STELLAR_H
#define SEQAN_HEADER_STELLAR_H

#include <iostream>
#include <seqan/seeds.h>
#include "stellar_types.h"
#include "stellar_extension.h"
#include "stellar_index.hpp"

#include "stellar/stellar.diagnostics.hpp"

namespace stellar
{

using namespace seqan;


struct VerifyAllLocal_;
typedef Tag<VerifyAllLocal_> const AllLocal;

struct VerifyBestLocal_;
typedef Tag<VerifyBestLocal_> const BestLocal;

struct VerifyBandedGlobal_;
typedef Tag<VerifyBandedGlobal_> const BandedGlobal;

struct VerifyBandedGlobalExtend_;
typedef Tag<VerifyBandedGlobalExtend_> const BandedGlobalExtend;

///////////////////////////////////////////////////////////////////////////////

inline bool _verifyFast(BestLocal) {
    return true;
}

template<typename TTag>
inline bool _verifyFast(TTag) {
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Appends a segment of only error positions from align to queue.
template<typename TAlign, typename TPos, typename TScoreValue>
inline void
_appendNegativeSegment(TAlign const & align,
                       TPos & pos, TPos const len,
                       Score<TScoreValue> const & scoreMatrix,
                       String<Triple<TPos, TPos, TScoreValue> > & queue) {
    typedef Triple<TPos, TPos, TScoreValue> TMerger;
    TPos beginPos = pos;

    TScoreValue score = 0;
    while (pos < len) {
        if (isGap(row(align, 0), pos) || isGap(row(align, 1), pos)) {
            score += scoreGap(scoreMatrix);
        } else if (value(row(align, 0), pos) != value(row(align, 1), pos)) {
            score += scoreMismatch(scoreMatrix);
        } else {
            break;
        }
        ++pos;
    }
    if (pos == len) appendValue(queue, TMerger(beginPos, pos, MinValue<TScoreValue>::VALUE + 1));
    else appendValue(queue, TMerger(beginPos, pos, score));
}

///////////////////////////////////////////////////////////////////////////////
// Appends a segment of only matching positions from align to queue.
template<typename TAlign, typename TPos, typename TScoreValue>
inline void
_appendPositiveSegment(TAlign const & align,
                       TPos & pos, TPos const len,
                       Score<TScoreValue> const & scoreMatrix,
                       String<Triple<TPos, TPos, TScoreValue> > & queue) {
    if (pos == len) return;
    typedef Triple<TPos, TPos, TScoreValue> TMerger;
    TPos beginPos = pos;

    TScoreValue score = 0;
    while ((pos < len) &&
           (!isGap(row(align, 0), pos) &&
            !isGap(row(align, 1), pos) &&
            (value(row(align, 0), pos) == value(row(align, 1), pos)))) {
        score += scoreMatch(scoreMatrix);
        ++pos;
    }
    appendValue(queue, TMerger(beginPos, pos, score));
}

///////////////////////////////////////////////////////////////////////////////
// See Lemma 5 in Zhang et al., 1999.
template<typename TMerger>
inline bool
_negativeMerge(String<TMerger> & queue) {
    typedef typename Value<TMerger, 1>::Type TPos;
    TPos len = length(queue);
    if (len < 3) return false;

    TMerger cd = value(queue, len-1);
    TMerger bc = value(queue, len-2);
    TMerger ab = value(queue, len-3);

    if ((bc.i3 < 0) || (bc.i3 >= abs(_max(ab.i3, cd.i3)))) {
        return false;
    } else {
        String<TMerger> newMerger;
        appendValue(newMerger, TMerger(ab.i1, cd.i2, ab.i3 + bc.i3 + cd.i3));
        replace(queue, len-3, len, newMerger);

        return true;
    }
}

///////////////////////////////////////////////////////////////////////////////
// See Lemma 6 in Zhang et al., 1999.
template<typename TMerger>
inline bool
_positiveMerge(String<TMerger> & queue) {
    typedef typename Value<TMerger, 1>::Type TPos;
    TPos len = length(queue);
    if (len < 5) return false;

    TMerger ef = value(queue, len-1);
    TMerger de = value(queue, len-2);
    TMerger cd = value(queue, len-3);
    TMerger bc = value(queue, len-4);
    TMerger ab = value(queue, len-5);

    if ((cd.i3 >= 0) || (cd.i3 < _max(ab.i3, ef.i3))) {
        return false;
    } else {
        String<TMerger> newMerger;
        appendValue(newMerger, TMerger(bc.i1, de.i2, bc.i3 + cd.i3 + de.i3));
        replace(queue, len-4, len-1, newMerger);

        return true;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Implements the algorithm from Zhang et al. in Bioinformatics, 1999: "Post-processing long pairwise alignments".
// Splits an alignment into sub-alignments that contain no x-Drop.
template<typename TAlign, typename TScoreValue, typename TScoreValue1, typename TScoreValue2>
void
_splitAtXDrops(TAlign const & align,
               Score<TScoreValue> const & scoreMatrix,
               TScoreValue1 const scoreDropOff,
               TScoreValue2 const minScore,
               String<TAlign> & alignmentString) {
    typedef typename Position<Row<TAlign> >::Type TPos;
    typedef Triple<TPos, TPos, TScoreValue> TMerger;

    // initialization
    String<TMerger> queue;
    TPos pos = _min(toViewPosition(row(align, 0), beginPosition(row(align, 0))),
                    toViewPosition(row(align, 1), beginPosition(row(align, 1))));
    appendValue(queue, TMerger(pos, pos, MinValue<TScoreValue1>::VALUE + 1));

    TPos aliLength = _max(toViewPosition(row(align, 0), endPosition(row(align, 0))),
                          toViewPosition(row(align, 1), endPosition(row(align, 1))));
    TPos len;
    while ((pos < aliLength) || (length(queue) > 1)) {
        // construct useful tree
        if (!_negativeMerge(queue)) {
            if (!_positiveMerge(queue)) {
                _appendPositiveSegment(align, pos, aliLength, scoreMatrix, queue);
                _appendNegativeSegment(align, pos, aliLength, scoreMatrix, queue);
            }
        }

        // check for x-Drop
        len = length(queue);
        if ((len == 3) && (value(queue, 2).i3 < scoreDropOff * (-1))) {
            if (value(queue, 1).i3 >= minScore) {
                // create new sub-alignment
                TAlign ali(align);
                // std::cerr << "SEQ0 " << source(row(ali, 0)) << "\n";
                // std::cerr << "SEQ1 " << source(row(ali, 1)) << "\n";
                // std::cerr << "ROW0\n" << row(ali, 0) << "\nqueue[1].i1 == " << queue[1].i1 << ", queue[1].i2 == " << queue[1].i2 << "\n";
                // std::cerr << "ROW1\n" << row(ali, 1) << "\nqueue[1].i1 == " << queue[1].i1 << ", queue[1].i2 == " << queue[1].i2 <<
                        // "\n";
                setClippedBeginPosition(row(ali, 0), queue[1].i1 + clippedBeginPosition(row(align, 0)));
                setClippedBeginPosition(row(ali, 1), queue[1].i1 + clippedBeginPosition(row(align, 1)));
                setClippedEndPosition(row(ali, 0), queue[1].i2 + clippedBeginPosition(row(align, 0)));
                setClippedEndPosition(row(ali, 1), queue[1].i2 + clippedBeginPosition(row(align, 1)));

                // TPos begin0 = toSourcePosition(row(ali, 0), value(queue, 1).i1);
                // TPos begin1 = toSourcePosition(row(ali, 1), value(queue, 1).i1);
                // TPos end0 = toSourcePosition(row(ali, 0), value(queue, 1).i2);
                // TPos end1 = toSourcePosition(row(ali, 1), value(queue, 1).i2);
                // setClippedBeginPosition(row(ali, 0), begin0);
                // setClippedBeginPosition(row(ali, 1), begin1);
                // setBeginPosition(row(ali, 0), 0);
                // setBeginPosition(row(ali, 1), 0);
                // setClippedEndPosition(row(ali, 0), end0);
                // setClippedEndPosition(row(ali, 1), end1);

                // append sub-alignment
                appendValue(alignmentString, ali);
            }
            replace(queue, 0, 2, String<TMerger>());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Checks whether two matches overlap *in seq2* and
//  whether the non-overlaping parts are shorter than minLength.
template<typename TMatch, typename TSize>
bool
checkOverlap(TMatch const & matchA, TMatch const & matchB, TSize const minLength) {
    // check id and orienation
    if (matchA.id != matchB.id || matchA.orientation != matchB.orientation) return false;
    if (matchA.id == TMatch::INVALID_ID || matchB.id == TMatch::INVALID_ID) return false;

    // check overlap in seq2
    if (matchA.begin2 >= matchB.begin2) {
        if (matchA.end2 >= matchB.end2) {
            // check length of non-overlapping parts of both matches
            if ((TSize)matchA.begin2 - (TSize)matchB.begin2 >= minLength &&
                (TSize)matchA.end2 - (TSize)matchB.end2 >= minLength) {
                return false;
            }
        }
        // check whether offset is the same in both sequences
        if (toViewPosition(matchA.row2, matchA.begin2) - toViewPosition(matchB.row2, matchB.begin2) !=
            toViewPosition(matchA.row1, matchA.begin1) - toViewPosition(matchB.row1, matchB.begin1)) {
            return false;
        }
    } else {
        if (matchA.end2 < matchB.end2) {
            // check length of non-overlapping parts of both matches
            if ((TSize)matchB.begin2 - (TSize)matchA.begin2 >= minLength &&
                (TSize)matchB.end2 - (TSize)matchA.end2 >= minLength) {
                return false;
            }
        }
        // check whether offset is the same in both sequences
        if (toViewPosition(matchB.row2, matchB.begin2) - toViewPosition(matchA.row2, matchA.begin2) !=
            toViewPosition(matchB.row1, matchB.begin1) - toViewPosition(matchA.row1, matchA.begin1)) {
            return false;
        }
    }
    return true;
}

template<typename TRow, typename TPosition>
TPosition
projectedPosition(TRow const & rowA, TRow const & rowB, TPosition pos)
{
    return toSourcePosition(rowB, toViewPosition(rowA, pos));
}

///////////////////////////////////////////////////////////////////////////////
// Checks all alignment columns of two overlapping matches.
// It is assumed that matchA.begin1 < matchB.begin1.
template<typename TMatch, typename TSize>
bool
_checkAlignColOverlap(TMatch const & matchA, TMatch const & matchB, TSize const minLength)
{
    TSize equalCols = 0;
    TSize diffCols = 0;

    for  (typename TMatch::TPos pos = matchB.begin1; pos < _min(matchA.end1, matchB.end1); ++pos)
    {
        if (projectedPosition(matchA.row1, matchA.row2, pos) == projectedPosition(matchB.row1, matchB.row2, pos))
            ++equalCols;
        else
            ++diffCols;
    }

    if (diffCols >= minLength) return false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Marks matches that overlap in both sequences with a longer match as invalid.
template<typename TSequence, typename TId, typename TSize>
void maskOverlaps(String<StellarMatch<TSequence const, TId> > & matches, TSize const minLength)
{
    typedef StellarMatch<TSequence const, TId>              TMatch;
    typedef typename TMatch::TPos                           TPos;
    typedef typename Iterator<String<TMatch>, Rooted>::Type TIter;
    typedef typename Iterator<String<TSize>, Rooted>::Type  TOverlapIter;

    // sort matches by begin position in row0
    sortMatches(matches, LessPos<TMatch>());

    // iterate all matches
    TIter it = begin(matches);

    // list of matches that potentially overlap with the current match in row0 and
    // start earlier (including matches that overlap but have a unique part of at
    // least minLength) sorted by descending end positions
    String<TSize> overlaps;

    for (; it != end(matches); ++it)
    {
        if ((*it).id == TMatch::INVALID_ID) continue;

        TPos insertPos = 0;

        // iterate potentially overlapping matches
        TOverlapIter overlapIt = begin(overlaps);
        for (; overlapIt != end(overlaps); ++overlapIt)
        {
            TMatch & o = matches[*overlapIt];

            // determine position for inserting *it into overlaps after checking
            if ((*it).end1 < o.end1) insertPos++;

            // check if matches overlap in row0 - if not, then break
            if (o.end1 <= (*it).begin1) break;

            // check if unique parts of the two matches in row0 are longer than minLength - if yes, then continue
            if ((*it).begin1 - o.begin1 >= (TPos)minLength &&
                (*it).end1 > o.end1 && (*it).end1 - o.end1 >= (TPos)minLength) continue;

            // check if matches overlap in row1 - if not, then continue
            if (!checkOverlap(*it, o, minLength)) continue;

            // check exact alignment columns for overlap
            if (!_checkAlignColOverlap(o, *it, minLength)) continue;

            // set shorter match invalid
            if (length(*it) > length(o))
                o.id = TMatch::INVALID_ID;
            else
                (*it).id = TMatch::INVALID_ID;
        }

        // remove all matches from overlaps that end earlier than current match begins
        resize(overlaps, position(overlapIt));

        if ((*it).id != TMatch::INVALID_ID)
            insertValue(overlaps, insertPos, position(it));
    }

}

///////////////////////////////////////////////////////////////////////////////
// Removes matches that are marked as invalid, and then keeps only the numMatches best matches.
template<typename TSequence, typename TId, typename TSize>
void
compactMatches(String<StellarMatch<TSequence const, TId> > & matches, TSize const numMatches) {
    typedef StellarMatch<TSequence const, TId>                  TMatch;
    typedef typename Iterator<String<TMatch>, Standard>::Type   TIterator;

    // sort matches by length (and validity)
    sortMatches(matches, LessLength<TMatch>());

    // count valid matches
    TSize num = 0;

    TIterator it = begin(matches, Standard());
    TIterator itEnd = end(matches, Standard());

    for(; it != itEnd; ++it) {
        if ((*it).id != TMatch::INVALID_ID)
            ++num;
    }

    // keep only valid and longest matches
    resize(matches, _min(num, numMatches));
}

template<typename TMatch_>
inline bool
QueryMatches<TMatch_>::
removeOverlapsAndCompactMatches(size_t const disableThresh,
                                size_t const compactThresh,
                                size_t const minLength,
                                size_t const numMatches)
{
    if (this->disabled)
        return false;

    size_t const matchesCount = length(this->matches);

    if (matchesCount > disableThresh) {
        this->disabled = true;
        clear(this->matches);
        return false;
    }

    if (matchesCount <= compactThresh)
        return false;

    maskOverlaps(this->matches, minLength);      // remove overlaps and duplicates
    compactMatches(this->matches, numMatches);   // keep only the <numMatches> longest matches
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Appends a match to matches container and removes overlapping matches if threshold is reached.
template<typename TSource, typename TId, typename TSize, typename TSize1>
inline bool
_insertMatch(QueryMatches<StellarMatch<TSource const, TId> > & queryMatches,
             StellarMatch<TSource const, TId> const & match,
             TSize const minLength,
             TSize1 const disableThresh,
             TSize1 & compactThresh,
             TSize1 const numMatches) {

    appendValue(queryMatches.matches, match);

    // std::cerr << "Inserting match \n-------------\n" << match.row1 <<"\n" << match.row2 << "----------------\n";

    if (queryMatches.removeOverlapsAndCompactMatches(disableThresh, compactThresh, minLength, numMatches))
    {
        // raise compact threshold if many matches are kept
        if ((length(queryMatches.matches) << 1) > compactThresh)
            compactThresh += (compactThresh >> 1);
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Conducts banded alignment on swift hit and extracts longest contained eps-match.
template<typename TSequence, typename TEpsilon, typename TSize, typename TDelta,
         typename TDrop, typename TSize1, typename TSource, typename TId>
void
verifySwiftHit(Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & infH,
               Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & infV,
               TEpsilon const eps,
               TSize const minLength,
               TDrop /*xDrop*/,
               TDelta const delta,
               TSize1 const disableThresh,
               TSize1 & compactThresh,
               TSize1 const numMatches,
               TId const & databaseId,
               bool const dbStrand,
               QueryMatches<StellarMatch<TSource const, TId> > & matches,
               BandedGlobal) {
    using TInfix = Segment<TSequence const, InfixSegment>;
    typedef Segment<TInfix, InfixSegment> TSegment;
    typedef typename StellarMatch<TSource const, TId>::TAlign TAlign;

    // define a scoring scheme
    typedef int TScore;
    TScore match = 1;
    TScore mismatchIndel = (TScore)_max((TScore) ceil(-1/eps) + 1, -(TScore)length(host(infH)));
    Score<TScore> scoreMatrix(match, mismatchIndel, mismatchIndel);

    // diagonals for banded alignment
    int64_t upperDiag = 0;
    int64_t lowerDiag = endPosition(infH) - (int64_t)endPosition(infV) - beginPosition(infH) + beginPosition(infV);
    if (beginPosition(infV) == 0) upperDiag = lowerDiag + delta;
    if (endPosition(infV) == endPosition(host(infV))) lowerDiag = -(int64_t)delta;

    // banded alignment on parallelogram
    Align<TSegment> bandedAlign;
    resize(rows(bandedAlign), 2);
    assignSource(row(bandedAlign, 0), infH);
    assignSource(row(bandedAlign, 1), infV);
    globalAlignment(bandedAlign, scoreMatrix, lowerDiag, upperDiag, NeedlemanWunsch());

    longestEpsMatch(bandedAlign, minLength, eps);

    // integrate alignment in object of type TAlign
    TAlign align;
    resize(rows(align), 2);
    setSource(row(align, 0), host(host(infH)));
    setSource(row(align, 1), host(host(infV)));
    integrateAlign(align, bandedAlign);

    // TODO(holtgrew): The following has not been adapted to the new clipping interface yet!
    // set begin and end positions of align
    SEQAN_FAIL("TODO(bkehr): Adapt to new clipping interface!");
    setClippedBeginPosition(row(align, 0), beginPosition(infH) + clippedBeginPosition(row(bandedAlign, 0)));
    setClippedBeginPosition(row(align, 1), beginPosition(infV) + beginPosition(host(infV)) + clippedBeginPosition(row(bandedAlign, 1)));
    setBeginPosition(row(align, 0), 0);
    setBeginPosition(row(align, 1), 0);
    setClippedEndPosition(row(align, 0), beginPosition(infH) + clippedEndPosition(row(bandedAlign, 0)));
    setClippedEndPosition(row(align, 1), beginPosition(infV) + beginPosition(host(infV)) + clippedEndPosition(row(bandedAlign, 1)));

    if ((TSize)length(row(align, 0)) < minLength)
        return;

    // insert eps-match in matches string
    StellarMatch<TSource const, TId> m(align, databaseId, dbStrand);
    _insertMatch(matches, m, minLength, disableThresh, compactThresh, numMatches);
}

///////////////////////////////////////////////////////////////////////////////
// Conducts banded alignment on swift hit, extends alignment, and extracts longest contained eps-match.
template<typename TSequence, typename TEpsilon, typename TSize, typename TDelta,
         typename TDrop, typename TSize1, typename TSource, typename TId>
void
verifySwiftHit(Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & infH,
               Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & infV,
               TEpsilon const eps,
               TSize const minLength,
               TDrop const xDrop,
               TDelta const delta,
               TSize1 const disableThresh,
               TSize1 & compactThresh,
               TSize1 const numMatches,
               TId const & databaseId,
               bool const dbStrand,
               QueryMatches<StellarMatch<TSource const, TId> > & matches,
               BandedGlobalExtend) {
    using TInfix = Segment<TSequence const, InfixSegment>;
    typedef Segment<TInfix, InfixSegment> TSegment;
    typedef typename StellarMatch<TSource const, TId>::TAlign TAlign;

    // define a scoring scheme
    typedef int TScore;
    TScore match = 1;
    TScore mismatchIndel = (TScore)_max((TScore) ceil(-1/eps) + 1, -(TScore)length(host(infH)));
    Score<TScore> scoreMatrix(match, mismatchIndel, mismatchIndel);
    TScore scoreDropOff = (TScore) _max((TScore) xDrop * (-mismatchIndel), MinValue<TScore>::VALUE + 1);

    // diagonals for banded alignment
    int64_t upperDiag = 0;
    int64_t lowerDiag = endPosition(infH) - (int64_t)endPosition(infV) - beginPosition(infH) + beginPosition(infV);
    if (beginPosition(infV) == 0) upperDiag = lowerDiag + delta;
    if (endPosition(infV) == endPosition(host(infV))) lowerDiag = -(int64_t)delta;

    // banded alignment on parallelogram
    Align<TSegment> bandedAlign;
    resize(rows(bandedAlign), 2);
    assignSource(row(bandedAlign, 0), infH);
    assignSource(row(bandedAlign, 1), infV);
    globalAlignment(bandedAlign, scoreMatrix, lowerDiag, upperDiag, NeedlemanWunsch());

    // create alignment object for the complete sequences
    TAlign align;
    resize(rows(align), 2);
    setSource(row(align, 0), host(host(infH)));
    setSource(row(align, 1), host(host(infV)));

    // extend alignment and obtain longest contained eps-match
    // TODO: something is wrong here, e.g. extract around seed, but also something else
    if (!_extendAndExtract(bandedAlign, scoreDropOff, scoreMatrix, infH, infV, EXTEND_BOTH, minLength, eps, align))
        return;

    // insert eps-match in matches string
    StellarMatch<TSource const, TId> m(align, databaseId, dbStrand);
    _insertMatch(matches, m, minLength, disableThresh, compactThresh, numMatches);
}

///////////////////////////////////////////////////////////////////////////////
// Conducts banded local alignment on swift hit (= computes eps-cores),
//  splits eps-cores at X-drops, and calls _extendAndExtract for extension of eps-cores
template<typename TSequence, typename TEpsilon, typename TSize, typename TDelta, typename TDrop,
         typename TSize1, typename TId, typename TSource, typename TTag>
void
verifySwiftHit(Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & infH,
               Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & infV,
               TEpsilon const eps,
               TSize const minLength,
               TDrop const xDrop,
               TDelta const delta,
               TSize1 const disableThresh,
               TSize1 & compactThresh,
               TSize1 const numMatches,
               TId const & databaseId,
               bool const dbStrand,
               QueryMatches<StellarMatch<TSource const, TId> > & matches,
               TTag tag) {
    using TInfix = Segment<TSequence const, InfixSegment>;
    typedef Segment<TInfix, InfixSegment> TSegment;
    typedef typename StellarMatch<TSource const, TId>::TAlign TAlign;

    TSize maxLength = 1000000000;
    if ((TSize)length(infH) > maxLength) {
        std::cerr << "Warning: SWIFT hit <" << beginPosition(infH) << "," << endPosition(infH);
        std::cerr << "> , <" << beginPosition(infV) << "," << endPosition(infV);
        std::cerr << "> too long. Verification skipped.\n" << std::flush;
        return;
    }

    // define a scoring scheme
    typedef int TScore;
    TScore match = 1;
    TScore mismatchIndel = (TScore)_max((TScore) ceil(-1/eps) + 1, -(TScore)length(host(infH)));
    Score<TScore> scoreMatrix(match, mismatchIndel, mismatchIndel);
    TScore scoreDropOff = (TScore) _max((TScore) xDrop * (-mismatchIndel), MinValue<TScore>::VALUE + 1);

    // calculate minimal score for local alignments
    TEpsilon e = floor(eps*minLength);
    TSize minLength1 = _max(0, (TSize)ceil((e+1) / eps));
    TEpsilon e1 = floor(eps*minLength1);
    TSize minScore = _min((TSize)ceil((minLength-e) / (e+1)), (TSize)ceil((minLength1-e1) / (e1+1)));

    // diagonals for banded local alignment
    int64_t upperDiag = 0;
    int64_t lowerDiag = endPosition(infH) - (int64_t)endPosition(infV) - beginPosition(infH) + beginPosition(infV);
    if (beginPosition(infV) == 0) {
        if (endPosition(infV) == endPosition(host(infV))) {
            // TODO: is it possible to get a smaller band in this case?
            upperDiag = delta;
            lowerDiag = -(int64_t)delta;
        } else
            upperDiag = lowerDiag + delta;
    } else if (endPosition(infV) == endPosition(host(infV)))
        lowerDiag = -(int64_t)delta;

    // banded local alignment
    LocalAlignmentEnumerator<Score<TScore>, Banded> enumerator(scoreMatrix, lowerDiag, upperDiag, minScore);
    Align<TSegment> localAlign;
    resize(rows(localAlign), 2);
    assignSource(row(localAlign, 0), infH);
    assignSource(row(localAlign, 1), infV);

    while (nextLocalAlignment(localAlign, enumerator)) {
    // while (localAlignment(localAlign, finder, scoreMatrix, minScore, lowerDiag, upperDiag, BandedWatermanEggert())) {

        // std::cerr << "localAlign == \n" << localAlign << "\n";

        // split local alignments containing an X-drop
        String<Align<TSegment> > seedAlignments;
        _splitAtXDrops(localAlign, scoreMatrix, scoreDropOff, minScore, seedAlignments);

        typename Iterator<String<Align<TSegment> > >::Type aliIt = begin(seedAlignments);
        while (aliIt != end(seedAlignments)) {
            // std::cerr << "aliIt == \n" << row(*aliIt, 0) << "\n" << row(*aliIt, 1) << "\n";
            // create alignment object for the complete sequences
            TAlign align;
            resize(rows(align), 2);
            setSource(row(align, 0), host(host(infH)));
            setSource(row(align, 1), host(host(infV)));

            // determine extension direction
            ExtensionDirection direction;
            if (length(seedAlignments) == 1) direction = EXTEND_BOTH;
            else if (aliIt == begin(seedAlignments)) direction = EXTEND_RIGHT;
            else if (aliIt == end(seedAlignments)-1) direction = EXTEND_LEFT;
            else direction = EXTEND_NONE;

            // extend alignment and obtain longest contained eps-match
            if (!_extendAndExtract(*aliIt, scoreDropOff, scoreMatrix, infH, infV, direction, minLength, eps, align)) {
                aliIt++;
                continue;
            }

            // insert eps-match in matches string
            StellarMatch<TSource const, TId> m(align, databaseId, dbStrand);
            length(m);  // DEBUG: Contains assertion on clipping.
            if(!_insertMatch(matches, m, minLength, disableThresh, compactThresh, numMatches)) return;
            ++aliIt;
        }
        if (_verifyFast(tag)) break;
    }
}

template <typename TVerifierTag>
struct SwiftHitVerifier
{
    using TSize = int;
    using TDrop = double;
    using TSize1 = unsigned;
    using TId = CharString;

    double const epsilon;
    TSize const minLength;
    TDrop const xDrop;
    TSize1 const disableThresh;
    TSize1 & compactThresh; // will be updated in _insertMatch
    TSize1 const numMatches;
    TId const & databaseID;
    bool const databaseStrand;

    template <typename TSequence, typename TDelta, typename TSource, typename TId>
    void verify(Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & finderSegment,
                Segment<Segment<TSequence const, InfixSegment>, InfixSegment> const & patternSegment,
                TDelta const delta,
                QueryMatches<StellarMatch<TSource const, TId> > & queryMatches)
    {
        verifySwiftHit(finderSegment, patternSegment, epsilon, minLength, xDrop,
                       delta, disableThresh, compactThresh,
                       numMatches, databaseID, databaseStrand, queryMatches, TVerifierTag{});
    }
};

///////////////////////////////////////////////////////////////////////////////
// Calls swift filter and verifies swift hits. = Computes eps-matches.
// A basic block for stellar
template<typename TAlphabet, typename TSource, typename TId, typename TTag>
StellarComputeStatistics
_stellarKernel(StellarSwiftFinder<TAlphabet> & finder,
               StellarSwiftPattern<TAlphabet> & pattern,
               StringSet<QueryMatches<StellarMatch<TSource const, TId> > > & matches,
               SwiftHitVerifier<TTag> & swiftVerifier) {
    using TText = String<TAlphabet>;
    using TMatch = StellarMatch<TSource const, TId>;
    using TInfix = typename Infix<TText const>::Type;

    StellarComputeStatistics statistics{};

    while (find(finder, pattern, swiftVerifier.epsilon, swiftVerifier.minLength)) {
        TInfix const finderInfix = infix(finder);
        TInfix const finderInfixSeq = infix(haystack(finder), 0, length(haystack(finder)));
        Segment<TInfix, InfixSegment> finderSegment(finderInfixSeq,
            beginPosition(finderInfix) - beginPosition(haystack(finder)),
            endPosition(finderInfix) - beginPosition(haystack(finder)));

        ++statistics.numSwiftHits;
        statistics.totalLength += length(finderInfix);
        statistics.maxLength = std::max<size_t>(statistics.maxLength, length(finderInfix));

        QueryMatches<TMatch> & queryMatches = value(matches, pattern.curSeqNo);

        if (queryMatches.disabled) continue;

        TText const & patternSeq = getSequenceByNo(pattern.curSeqNo, indexText(needle(pattern)));
        TInfix const patternInfix = infix(pattern, patternSeq);
        TInfix const patternInfixSeq = infix(patternSeq, 0, length(patternSeq));
        Segment<TInfix, InfixSegment> patternSegment(patternInfixSeq,
            beginPosition(patternInfix) - beginPosition(patternSeq),
            endPosition(patternInfix) - beginPosition(patternSeq));

        ////Debug stuff:
        //std::cout << beginPosition(finderInfix) << ",";
        //std::cout << endPosition(finderInfix) << "  ";
        //std::cout << beginPosition(patternSegment) << ",";
        //std::cout << endPosition(patternSegment) << std::endl;

        // verification
        swiftVerifier.verify(finderSegment,
                             patternSegment,
                             pattern.bucketParams[0].delta + pattern.bucketParams[0].overlap,
                             queryMatches);
    }

    return statistics;
}

///////////////////////////////////////////////////////////////////////////////
// Calls swift filter and verifies swift hits. = Computes eps-matches.
template<typename TAlphabet, typename TSize, typename TDrop, typename TSize1,
         typename TMode, typename TSource, typename TId, typename TTag>
void stellar(StellarSwiftFinder<TAlphabet> & finder,
             StellarSwiftPattern<TAlphabet> & pattern,
             double const epsilon,
             TSize const minLength,
             TDrop const xDrop,
             TSize1 const disableThresh,
             TSize1 & compactThresh,
             TSize1 const numMatches,
             TMode const verbose,
             TId const & databaseID,
             bool const databaseStrand,
             StringSet<QueryMatches<StellarMatch<TSource const, TId> > > & matches,
             TTag /*tag*/) {

    SwiftHitVerifier<TTag> swiftVerifier
    {
        STELLAR_DESIGNATED_INITIALIZER(.epsilon =, epsilon),
        STELLAR_DESIGNATED_INITIALIZER(.minLength =, minLength),
        STELLAR_DESIGNATED_INITIALIZER(.xDrop =, xDrop),
        STELLAR_DESIGNATED_INITIALIZER(.disableThresh =, disableThresh),
        STELLAR_DESIGNATED_INITIALIZER(.compactThresh =, compactThresh),
        STELLAR_DESIGNATED_INITIALIZER(.numMatches =, numMatches),
        STELLAR_DESIGNATED_INITIALIZER(.databaseID =, databaseID),
        STELLAR_DESIGNATED_INITIALIZER(.databaseStrand =, databaseStrand)
    };

    StellarComputeStatistics statistics = _stellarKernel(finder, pattern, matches, swiftVerifier);

    if (verbose)
        stellar::app::_printStellarKernelStatistics(statistics);

    using TMatch = StellarMatch<TSource const, TId>;
    for (QueryMatches<TMatch> & queryMatches : matches) {
        removeOverlapsAndCompactMatches(queryMatches, disableThresh, /*compactThresh*/ 0, minLength, numMatches);
    }
}

// Wrapper for stellar
template<typename TAlphabet, typename TSize, typename TDrop,
         typename TSource, typename TId, typename TTag>
void stellar(StellarSwiftFinder<TAlphabet> & finder,
             StellarSwiftPattern<TAlphabet> & pattern,
             double const epsilon,
             TSize const minLength,
             TDrop const xDrop,
             StringSet<QueryMatches<StellarMatch<TSource const, TId> > > & matches,
             TTag tag) {
    unsigned const maxValue = (unsigned)-1;
    unsigned const disableThresh = maxValue;
    unsigned const numMatches = disableThresh;
    unsigned compactThresh = 1000;
    TId const id = "db";

    stellar(finder, pattern, epsilon, minLength, xDrop,
            disableThresh, compactThresh, numMatches, /*verbose*/ false, id, /*databaseStrand*/ true,
            matches, tag);
}

} // namespace stellar

#endif
