// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2013, Knut Reinert, FU Berlin
// Copyright (c) 2013 NVIDIA Corporation
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
// Author: Jochen Singer <jochen.singer@fu-berlin.de>
// ==========================================================================

//SEQAN_NO_DDDOC:do not generate documentation for this file


#ifndef INDEX_FM_COMPRESSED_SA_H_
#define INDEX_FM_COMPRESSED_SA_H_

namespace seqan {

// ============================================================================
// Forwards
// ============================================================================

template <typename TText, typename TSpec, typename TConfig>
struct CompressedSA;

template <typename TText, typename TSpec, typename TConfig>
struct LF;

struct FibreLF_;
typedef Tag<FibreLF_> const     FibreLF;

// ============================================================================
// Tags
// ============================================================================

/**
.Tag.CompressedSA Fibres
..summary:Tag to select a specific fibre of a @Class.CompressedSA@.
..remarks:These tags can be used to get @Metafunction.Fibre.Fibres@ of a sparse string.
..cat:Index

..tag.FibreSparseString:The sparse string.

..see:Metafunction.Fibre
..see:Function.getFibre
..include:seqan/index.h
*/
// TODO(esiragusa): Rename FibreSparseString as FibreSparseValues.
struct FibreSparseString_;
typedef Tag<FibreSparseString_> const FibreSparseString;

// ============================================================================
// Metafunctions
// ============================================================================

// ----------------------------------------------------------------------------
// Metafunction Fibre
// ----------------------------------------------------------------------------

template <typename TText, typename TSpec, typename TConfig>
struct Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>
{
    // TODO(esiragusa): Change SparseString spec to be SparseString<TValue, TSpec, TConfig>.
    typedef typename SAValue<TText>::Type               TSAValue_;
    typedef SparseString<String<TSAValue_>, TSpec>      Type;
};

template <typename TText, typename TSpec, typename TConfig>
struct Fibre<CompressedSA<TText, TSpec, TConfig>, FibreLF>
{
    typedef LF<TText, TSpec, TConfig>   Type;
};

// ----------------------------------------------------------------------------
// Metafunction Member
// ----------------------------------------------------------------------------

template <typename TText, typename TSpec, typename TConfig>
struct Member<CompressedSA<TText, TSpec, TConfig>, FibreLF>
{
    typedef Holder<typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreLF>::Type>   Type;
};

// ----------------------------------------------------------------------------
// Metafunction Reference
// ----------------------------------------------------------------------------

template <typename TText, typename TSpec, typename TConfig>
struct Reference<CompressedSA<TText, TSpec, TConfig> >
{
    // TODO(singer): We actually need a proxy here.
    typedef typename Value<CompressedSA<TText, TSpec, TConfig> >::Type Type;
};

template <typename TText, typename TSpec, typename TConfig>
struct Reference<CompressedSA<TText, TSpec, TConfig> const>
{
    // TODO(singer): We actually need a proxy here.
    typedef typename Value<CompressedSA<TText, TSpec, TConfig> >::Type /*const*/ Type;
};

// ----------------------------------------------------------------------------
// Metafunction Value
// ----------------------------------------------------------------------------

template <typename TText, typename TSpec, typename TConfig>
struct Value<CompressedSA<TText, TSpec, TConfig> >
{
    typedef typename Value<typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type>::Type   Type;
};

// ============================================================================
// Classes
// ============================================================================

// ----------------------------------------------------------------------------
// Class CompressedSA
// ----------------------------------------------------------------------------

/**
.Class.CompressedSA:
..cat:Index
..summary:A suffix array storing only a few suffix array entries and computing the remaining on demand.
..signature:CompressedSA<TText, TSpec, TConfig>
..param.TSparseString:The string containing specific suffix array entries.
...type:Class.SparseString
..param.TLF:The lf containg an occurrence table and a prefix sum table.
...type:Class.LF
..param.TSpec:Possibility to specialise a compressed suffix array.
...default:void.
..remarks:The compressed suffix array can only be used with the FM index.
..include:seqan/index.h
*/

template <typename TText, typename TSpec, typename TConfig>
struct CompressedSA
{
    typename Fibre<CompressedSA, FibreSparseString>::Type   sparseString;
    typename Member<CompressedSA, FibreLF>::Type            lf;

    // NOTE(esiragusa): NVCC cyclic SEQAN_HOST_DEVICE problem.
//    CompressedSA() {};
//
//    CompressedSA(TLF const & lf) :
//        lf(lf)
//    {}

    template <typename TPos>
    SEQAN_HOST_DEVICE inline typename Value<CompressedSA>::Type const
    operator[](TPos pos)
    {
        return value(*this, pos);
    }

    template <typename TPos>
    SEQAN_HOST_DEVICE inline typename Value<CompressedSA>::Type
    operator[](TPos pos) const
    {
        return value(*this, pos);
    }
};

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function clear()
// ----------------------------------------------------------------------------

/**
.Function.clear.param.object.type:Class.CompressedSA
*/
template <typename TText, typename TSpec, typename TConfig>
inline void clear(CompressedSA<TText, TSpec, TConfig> & compressedSA)
{
    clear(getFibre(compressedSA, FibreSparseString()));
//    clear(getFibre(compressedSA, FibreLF()));
}

// ----------------------------------------------------------------------------
// Function empty()
// ----------------------------------------------------------------------------

/**
.Function.empty.param.object.type:Class.CompressedSA
*/
template <typename TText, typename TSpec, typename TConfig>
SEQAN_HOST_DEVICE inline bool empty(CompressedSA<TText, TSpec, TConfig> & compressedSA)
{
    return empty(getFibre(compressedSA, FibreSparseString()));
    //    && empty(getFibre(compressedSA, FibreLF()));
}

// ----------------------------------------------------------------------------
// Function createCompressedSa()
// ----------------------------------------------------------------------------
// This function creates a compressed suffix array using a normal one.

/**
.Function.CompressedSA#createCompressedSa
..summary:This functions creates a compressed suffix array with a specified compression factor.
..signature:void createCompressedSa(compressedSA, completeSA, compressionFactor [,offset])
..param.compressedSA:The compressed suffix array
...type:Class.CompressedSA
..param.completeSA:A complete suffix array containing all values
..param.compressionFactor:The compression factor.
...type:Concept.UnsignedIntegerConcept
...remarks:A compression factor of x means that the compressed suffix array specifically stores a value for every x values in the complete suffix array.
..param:offset:Number of elements at the beginning which should contain the default value.
..include:seqan/index.h
*/
template <typename TText, typename TSpec, typename TConfig, typename TSA, typename TSize>
void createCompressedSa(CompressedSA<TText, TSpec, TConfig> & compressedSA, TSA const & sa, TSize offset)
{
    typedef CompressedSA<TText, TSpec, TConfig>                     TCompressedSA;
    typedef typename Size<TSA>::Type                                TSASize;
    typedef typename Fibre<TCompressedSA, FibreSparseString>::Type  TSparseSA;
    typedef typename Fibre<TSparseSA, FibreIndicators>::Type        TIndicators;
    typedef typename Fibre<TSparseSA, FibreValues>::Type            TValues;
    typedef typename Iterator<TSA const, Standard>::Type            TSAIter;

    TSparseSA & sparseString = getFibre(compressedSA, FibreSparseString());
    TIndicators & indicators = getFibre(sparseString, FibreIndicators());
    TValues & values = getFibre(sparseString, FibreValues());

    TSASize saLen = length(sa);
    resize(compressedSA, saLen + offset, Exact());
    
    TSAIter saIt = begin(sa, Standard());
    TSAIter saItEnd = end(sa, Standard());

    for (TSASize pos = offset; saIt != saItEnd; ++saIt, ++pos)
    {
        if (getSeqOffset(getValue(saIt)) % TConfig::SAMPLING == 0)
            setValue(indicators, pos, true);
        else
            setValue(indicators, pos, false);
    }
    updateRanks(indicators);

    resize(values, getRank(indicators, length(sparseString) - 1), Exact());

    saIt = begin(sa, Standard());
    for (TSASize pos = offset, counter = 0; saIt != saItEnd; ++saIt, ++pos)
    {
        if (getValue(indicators, pos))
        {
            assignValue(values, counter, getValue(saIt));
            ++counter;
        }
    }
}

template <typename TText, typename TSpec, typename TConfig, typename TSA>
void createCompressedSa(CompressedSA<TText, TSpec, TConfig> & compressedSA, TSA const & sa)
{
    createCompressedSa(compressedSA, sa, 0);
}

// ----------------------------------------------------------------------------
// Function getFibre()
// ----------------------------------------------------------------------------

/**
.Function.CompressedSA#getFibre:
..summary:Returns a specific fibre of a container.
..signature:getFibre(container, fibreTag)
..class:Class.CompressedSA
..cat:Index
..param.container:The container holding the fibre.
...type:Class.CompressedSA
..param.fibreTag:A tag that identifies the @Metafunction.Fibre@.
...type:Tag.CompressedSA Fibres
..returns:A reference to the @Metafunction.Fibre@ object.
..include:seqan/index.h
..example.code:
Index< String<char> > index_esa("tobeornottobe");

String<char> & text = getFibre(indexEsa, EsaText());
*/

template <typename TText, typename TSpec, typename TConfig>
SEQAN_HOST_DEVICE inline typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type const &
getFibre(CompressedSA<TText, TSpec, TConfig> const & compressedSA, FibreSparseString)
{
    return compressedSA.sparseString;
}

template <typename TText, typename TSpec, typename TConfig>
SEQAN_HOST_DEVICE inline typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type &
getFibre(CompressedSA<TText, TSpec, TConfig> & compressedSA, FibreSparseString)
{
    return compressedSA.sparseString;
}

template <typename TText, typename TSpec, typename TConfig>
inline typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreLF>::Type const &
getFibre(CompressedSA<TText, TSpec, TConfig> const & compressedSA, FibreLF)
{
    return value(compressedSA.lf);
}

template <typename TText, typename TSpec, typename TConfig>
inline typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreLF>::Type &
getFibre(CompressedSA<TText, TSpec, TConfig> & compressedSA, FibreLF)
{
    return value(compressedSA.lf);
}

// ----------------------------------------------------------------------------
// Function setLfTable()
// ----------------------------------------------------------------------------
// NOTE(esiragusa): setLfTable() could be renamed as setFibre(csa, fibre, FibreLF()) or setHost(csa, fibre)

/**
.Function.setLfTable
..summary:Set the LF of the compressed suffix array.
..signature:setLfTable(CompressedSA<TText, TSpec, TConfig> compressedSa, TLF & lf)
..param.CompressedSA<TText, TSpec, TConfig>:The compressed suffix array.
...type:Class.CompressedSA
..param.lf
...type:Class.LF
..include:seqan/index.h
*/
template <typename TText, typename TSpec, typename TConfig, typename TLF>
void setLfTable(CompressedSA<TText, TSpec, TConfig> & compressedSA, TLF const & lf)
{
    setValue(compressedSA.lf, lf);
}

// ----------------------------------------------------------------------------
// Function length()
// ----------------------------------------------------------------------------

/**
.Function.length.param.object.type:Class.CompressedSA
*/
template <typename TText, typename TSpec, typename TConfig>
SEQAN_HOST_DEVICE inline typename Size<typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type>::Type
length(CompressedSA<TText, TSpec, TConfig> const & compressedSA)
{
    return length(getFibre(compressedSA, FibreSparseString()));
}

// ----------------------------------------------------------------------------
// Function resize()
// ----------------------------------------------------------------------------

/**
.Function.resize.param.object.type:Class.CompressedSA
*/
template <typename TText, typename TSpec, typename TConfig, typename TSize, typename TExpand>
inline typename Size<typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type>::Type
resize(CompressedSA<TText, TSpec, TConfig> & compressedSA, TSize size, Tag<TExpand> tag)
{
    return resize(getFibre(compressedSA, FibreSparseString()), size, tag);
}

// ----------------------------------------------------------------------------
// Function value()
// ----------------------------------------------------------------------------

/**
.Function.CompressedSA#value
..summary:Returns the value stored at a specified position in the compressed suffix-array.
..signature:value(compressedSA, pos)
..param.compressedSA:The compressed suffix array to access.
...type:Class.CompressedSA
..param.pos:Position at which to access the suffix array.
...type:Concept.UnsignedIntegerConcept
..remarks:Note that the compressed suffix array is read only. Therefore a const reference is return by
this function.
*/
template <typename TText, typename TSpec, typename TConfig, typename TPos>
SEQAN_HOST_DEVICE inline typename Value<CompressedSA<TText, TSpec, TConfig> >::Type
value(CompressedSA<TText, TSpec, TConfig> & compressedSA, TPos pos)
{
    typedef typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type     TSparseString;
    typedef typename Fibre<TSparseString, FibreIndicators>::Type    TIndicators;
    typedef typename Fibre<TSparseString, FibreValues>::Type        TValues;

    TIndicators const & indicators = getFibre(compressedSA.sparseString, FibreIndicators());
    TValues const & values = getFibre(compressedSA.sparseString, FibreValues());

    TPos counter = 0;
    for (; !getValue(indicators, pos); ++counter)
        pos = getFibre(compressedSA, FibreLF())(pos);

    return posAdd(getValue(values, getRank(indicators, pos) - 1), counter);
}

template <typename TText, typename TSpec, typename TConfig, typename TPos>
SEQAN_HOST_DEVICE inline typename Value<CompressedSA<TText, TSpec, TConfig> >::Type const
value(CompressedSA<TText, TSpec, TConfig> const & compressedSA, TPos pos)
{
    typedef typename Fibre<CompressedSA<TText, TSpec, TConfig>, FibreSparseString>::Type     TSparseString;
    typedef typename Fibre<TSparseString, FibreIndicators>::Type    TIndicators;
    typedef typename Fibre<TSparseString, FibreValues>::Type        TValues;

    TIndicators const & indicators = getFibre(compressedSA.sparseString, FibreIndicators());
    TValues const & values = getFibre(compressedSA.sparseString, FibreValues());

    TPos counter = 0;
    for (; !getValue(indicators, pos); ++counter)
        pos = getFibre(compressedSA, FibreLF())(pos);

    return posAdd(getValue(values, getRank(indicators, pos) - 1), counter);
}

// ----------------------------------------------------------------------------
// Function open()
// ----------------------------------------------------------------------------

/**
.Function.open
..param.object:
...type:Class.CompressedSA
*/
template <typename TText, typename TSpec, typename TConfig>
inline bool open(CompressedSA<TText, TSpec, TConfig> & compressedSA, const char * fileName, int openMode)
{
    return open(getFibre(compressedSA, FibreSparseString()), fileName, openMode);
}

template <typename TText, typename TSpec, typename TConfig>
inline bool open(CompressedSA<TText, TSpec, TConfig> & compressedSA, const char * fileName)
{
    return open(compressedSA, fileName, DefaultOpenMode<CompressedSA<TText, TSpec, TConfig> >::VALUE);
}

// ----------------------------------------------------------------------------
// Function save()
// ----------------------------------------------------------------------------

/**
.Function.CompressedSA#save
..class:Class.CompressedSA
..summary:This functions saves a compressed suffix array to disk.
..signature:open(compressedSA, fileName [, openMode])
..param.compressedSA:The string to be saved.
...type:Class.CompressedSA
..param.fileName:C-style character string containing the file name.
..param.openMode:The combination of flags defining how the file should be opened.
...remarks:To open a file read-only, write-only or to read and write use $OPEN_RDONLY$, $OPEN_WRONLY$, or $OPEN_RDWR$.
...remarks:To create or overwrite a file add $OPEN_CREATE$.
...remarks:To append a file if existing add $OPEN_APPEND$.
...remarks:To circumvent problems, files are always opened in binary mode.
...default:$OPEN_RDWR | OPEN_CREATE | OPEN_APPEND$
..returns:A $bool$ which is $true$ on success.
..include:seqan/index.h
*/
template <typename TText, typename TSpec, typename TConfig>
inline bool save(CompressedSA<TText, TSpec, TConfig> const & compressedSA, const char * fileName, int openMode)
{
    return save(getFibre(compressedSA, FibreSparseString()), fileName, openMode);
//  save(getFibre(compressedSA, FibreLF()), fileName, openMode);
}

template <typename TText, typename TSpec, typename TConfig>
inline bool save(CompressedSA<TText, TSpec, TConfig> const & compressedSA, const char * fileName)
{
    return save(compressedSA, fileName, DefaultOpenMode<CompressedSA<TText, TSpec, TConfig> >::VALUE);
}

}

#endif // INDEX_FM_COMPRESSED_SA_H_
