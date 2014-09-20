// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2013, Knut Reinert, FU Berlin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Manuel Holtgrewe <manuel.holtgrewe@fu-berlin.de>
// Author: David Weese <david.weese@fu-berlin.de>
// ==========================================================================
// The GenomicRegion class represents a region on one chromosome/contig in a
// genome, e.g. chr1, chr1:15,000, chr1:100,000-200,000.
// ==========================================================================

#ifndef CORE_INCLUDE_SEQAN_SEQ_IO_GENOMIC_REGION_H_
#define CORE_INCLUDE_SEQAN_SEQ_IO_GENOMIC_REGION_H_

namespace seqan {

// ============================================================================
// Forwards
// ============================================================================

struct GenomicRegion;
inline void parse(GenomicRegion & region, CharString const & regionString);

// ============================================================================
// Tags, Classes, Enums
// ============================================================================

// ---------------------------------------------------------------------------
// Class GenomicRegion
// ---------------------------------------------------------------------------

/*!
 * @class GenomicRegion
 * @headerfile <seqan/seq_io.h>
 * @brief Store information about a genomic region.
 * 
 * @signature class GenomicRegion;
 * 
 * A genomic region is a range on a chromosome.  The chromosome is identified by its name (as text in @link
 * GenomicRegion::seqName @endlink, optionally also as an <tt>integer</tt> in @link GenomicRegion::seqId @endlink).  The
 * range is stored as a half-open interval [@link GenomicRegion::beginPos @endlink, @link GenomicRegion::endPos @endlink).
 * If @link GenomicRegion::beginPos @endlink is set to <tt>-1</tt> then the range spans the whole chromosome.  If @link
 * GenomicRegion::beginPos @endlink is set to a value <tt>&gt;= 0</tt> and @link GenomicRegion::endPos @endlink is set ot
 * <tt>-1</tt>, then the chromosome is selected from @link GenomicRegion::beginPos @endlink to the end.
 * 
 * Examples for genomic regions are <tt>chr1</tt>, <tt>chr1:1,000</tt>, <tt>chr1:1,000-2,000</tt>.
 * 
 * The textual description of a genomic region has one of the formats <tt>NAME</tt>, <tt>NAME:START</tt>,
 * <tt>NAME:START-END</tt>.  The positions in the textual representation <tt>START</tt> and <tt>END</tt> are one-based.
 * However, the representation in the members of @link GenomicRegion @endlink is zero-based.
 * 
 * @section Examples
 * 
 * Construct a @link GenomicRegion @endlink object and fill it from different region strings.
 * 
 * @code{.cpp}
 * seqan::GenomicRegion genomicRegion;
 *  
 * parse(genomicRegion, "chr1");
 * // genomicRegion.seqName == "chr1"
 * // genomicRegion.seqId == -1, genomicRegion.beginPos == -1, genomicRegion.beginPos == -1
 *  
 * parse(genomicRegion, "chr1:1000");
 * // genomicRegion.seqName == "chr1"
 * // genomicRegion.beginPos == 999
 * // genomicRegion.seqId == -1, genomicRegion.beginPos == -1
 *  
 * parse(genomicRegion, "chr1:1000-2000");
 * // genomicRegion.seqName == "chr1"
 * // genomicRegion.beginPos == 999
 * // genomicRegion.beginPos == 2000
 * // genomicRegion.seqId == -1
 * @endcode
 *
 *
 * @fn GenomicRegion::GenomicRegion
 * @brief Constructor.
 * 
 * @signature GenomicRegion::GenomicRegion();
 * @signature GenomicRegion::GenomicRegion(str);
 * 
 * @param[in] str The string to parse region from. Types: CharString
 * 
 * The default constructor sets all integer members to <tt>-1</tt>, the <tt>seqName</tt> member is left empty.
 */

/*!
 * @var CharString GenomicRegion::seqName;
 * @brief Name of the sequence the region lies on, default is the empty string.
 * 
 * @var __int32 GenomicRegion::seqId;
 * @brief An optional field storing an integer.  Default is <tt>-1</tt>.
 * 
 * @var __int32 GenomicRegion::beginPos;
 * @brief Begin position of the range on the chromosome.  Default is <tt>-1</tt>.
 * 
 * @var __int32 GenomicRegion::endPos;
 * @brief End position of the range on the chromosome.  Default is <tt>-1</tt>.
 */

/**
.Class.GenomicRegion
..cat:Input/Output
..signature:GenomicRegion
..summary:Store information about a genomic region.
..description.text:
A genomic region is a range on a chromosome.
The chromosome is identified by its name (as text in @Memvar.GenomicRegion#seqName@, optionally also as an $integer$ in @Memvar.GenomicRegion#seqId@).
The range is stored as a half-open interval [@Memvar.GenomicRegion#beginPos@, @Memvar.GenomicRegion#endPos@).
If @Memvar.GenomicRegion#beginPos@ is set to $-1$ then the range spans the whole chromosome.
If @Memvar.GenomicRegion#beginPos@ is set to a value $>= 0$ and @Memvar.GenomicRegion#endPos@ is set ot $-1$, then the chromosome is selected from @Memvar.GenomicRegion#beginPos@ to the end.
..description.text:
Examples for genomic regions are $chr1$, $chr1:1,000$, $chr1:1,000-2,000$.
..description.text:
The textual description of a genomic region has one of the formats $NAME$, $NAME:START$, $NAME:START-END$.
The positions in the textual representation $START$ and $END$ are one-based.
However, the representation in the members of @Class.GenomicRegion@ is zero-based.
..example.text:Construct a @Class.GenomicRegion@ object and fill it from different region strings.
..example.code:
seqan::GenomicRegion genomicRegion;

parse(genomicRegion, "chr1");
// genomicRegion.seqName == "chr1"
// genomicRegion.seqId == -1, genomicRegion.beginPos == -1, genomicRegion.beginPos == -1

parse(genomicRegion, "chr1:1000");
// genomicRegion.seqName == "chr1"
// genomicRegion.beginPos == 999
// genomicRegion.seqId == -1, genomicRegion.beginPos == -1

parse(genomicRegion, "chr1:1000-2000");
// genomicRegion.seqName == "chr1"
// genomicRegion.beginPos == 999
// genomicRegion.beginPos == 2000
// genomicRegion.seqId == -1


..include:seqan/seq_io.h

.Memfunc.GenomicRegion#GenomicRegion
..class:Class.GenomicRegion
..summary:Constructor.
..description:
The default constructor sets all integer members to $-1$, the $seqName$ member is left empty.
..signature:GenomicRegion()
..signature:GenomicRegion(str)
..param.str:The string to parse region from.
...type:Shortcut.CharString

.Memvar.GenomicRegion#seqName
..class:Class.GenomicRegion
..summary:Name of the sequence the region lies on, default is the empty string.
..type:Shortcut.CharString

.Memvar.GenomicRegion#seqId
..class:Class.GenomicRegion
..summary:An optional field storing an integer. Default is $-1$.
..type:nolink:$__int32$

.Memvar.GenomicRegion#beginPos
..class:Class.GenomicRegion
..summary:Begin position of the range on the chromosome. Default is $-1$.
..type:nolink:$__int32$

.Memvar.GenomicRegion#endPos
..class:Class.GenomicRegion
..summary:End position of the range on the chromosome. Default is $-1$.
..type:nolink:$__int32$
*/

struct GenomicRegion
{
    // Name of sequence.
    CharString seqName;
    // Index of sequence in FASTA file.  -1 if not set.
    unsigned seqId;
    // 0-based begin position.  -1 if not set.
    unsigned beginPos;
    // 0-based, C-style end position.  -1 if not set.
    unsigned endPos;

    enum { INVALID_ID = (unsigned)-1 };
    enum { INVALID_POS = (unsigned)-1 };
//    static const unsigned INVALID_ID = -1;
//    static const unsigned INVALID_POS = -1;

    GenomicRegion() :
        seqId(INVALID_ID), beginPos(INVALID_POS), endPos(INVALID_POS)
    {}

    GenomicRegion(CharString const & str) :
        seqId(INVALID_ID), beginPos(INVALID_POS), endPos(INVALID_POS)
    {
        parse(*this, str);
    }
};

// ============================================================================
// Metafunctions
// ============================================================================

// ============================================================================
// Functions
// ============================================================================

// ---------------------------------------------------------------------------
// Function clear()
// ---------------------------------------------------------------------------

/*!
 * @fn GenomicRegion#clear
 * @brief Reset a GenomicRegion object to the same state after default construction.
 * 
 * @signature void clear(genomicRegion);
 * 
 * @param[in,out] genomicRegion The @link GenomicRegion @endlink object to reset.  Types: GenomicRegion
 */

/**
.Function.GenomicRegion#clear
..cat:Input/Output
..class:Class.GenomicRegion
..summary:Reset a @Class.GenomicRegion@ object to the same state after default construction.
..signature:reset(genomicRegion)
..param.genomicRegion:The @Class.GenomicRegion@ object to reset.
...type:Class.GenomicRegion
..returns:$void$, where $true$ indicates sucess
..include:seqan/seq_io.h
*/

inline void
clear(GenomicRegion & region)
{
    clear(region.seqName);
    region.seqId = GenomicRegion::INVALID_ID;
    region.beginPos = GenomicRegion::INVALID_POS;
    region.endPos = GenomicRegion::INVALID_POS;
}

// ---------------------------------------------------------------------------
// Function parse()
// ---------------------------------------------------------------------------

/*!
 * @fn GenomicRegion#parse
 * @brief Parse genomic region string store results in @link GenomicRegion @endlink.
 * 
 * @signature bool parse(genomicRegion, regionString);
 * 
 * @param[in]  regionString  The region string to prse.  Types: @link CharString @endlink.
 * @param[out] genomicRegion The @link GenomicRegion @endlink object to write the results to.
 *                           Types: GenomicRegion
 * 
 * @return bool true indicates successful parsing.
 */

/**
.Function.GenomicRegion#parse
..cat:Input/Output
..class:Class.GenomicRegion
..summary:Parse genomic region string store results in @Class.GenomicRegion@.
..signature:parse(genomicRegion, regionString)
..param.genomicRegion:The @Class.GenomicRegion@ object to write the results to.
...type:Class.GenomicRegion
..param.regionString:The region string to prse.
...type:Shortcut.CharString
..returns:$bool$, where $true$ indicates sucess
..example.text:See the example for parsing in the @Class.GenomicRegion@.
..include:seqan/seq_io.h
*/

// Parse regionString and write to region.  region.seqId will not be set but
// region.seqName will be.  Return true on success.

inline void
parse(GenomicRegion & region, CharString const & regionString)
{
    DirectionIterator<CharString const, Input>::Type reader = directionIterator(regionString, Input());

    // Parse out sequence name.
    clear(region.seqName);
    readUntil(region.seqName, reader, EqualsChar<':'>());
    if (atEnd(reader))
        return;

    skipOne(reader, EqualsChar<':'>());     // Skip ':'.

    // Parse out begin position.
    CharString buffer;
    readUntil(buffer, reader, EqualsChar<'-'>(), EqualsChar<','>());
    lexicalCastWithException(region.beginPos, buffer);

    if (region.beginPos < 1)
        SEQAN_THROW(ParseError("GenomicRegion: Begin postition less than 1"));

    region.beginPos--;                      // Adjust to 0-based.
    if (atEnd(reader))
        return;

    skipOne(reader, EqualsChar<'-'>());     // Skip '-'.

    // Parse out end position.
    clear(buffer);
    readUntil(buffer, reader, False(), EqualsChar<','>());
    lexicalCastWithException(region.endPos, buffer);

    if (region.endPos < 1)
        SEQAN_THROW(ParseError("GenomicRegion: End postition less than 1"));
}

}  // namespace seqan

#endif  // #ifndef CORE_INCLUDE_SEQAN_SEQ_IO_GENOMIC_REGION_H_
