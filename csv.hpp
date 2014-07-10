/////////////////////////////////////////////////////////////////////////
//                                                                     //
// This is a simple c++ class for one shot IO with CSV files           //
//                                                                     //
// Copyright (C) 2014 Alexandru Duliu                                  //
//                                                                     //
// This Source Code Form is subject to the terms of the Mozilla        //
// Public License v. 2.0. If a copy of the MPL was not distributed     //
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.  //
//                                                                     //
////////////////////////////////////////////////////////////////////////

#pragma once

///
/// \file    csv.hpp
/// \class   basic_csv
///
/// \package cvs
/// \version 0.2.3
///
/// \brief   simple c++ class for one shot IO with CSV files
///
/// \details this class is designed for simple IO to and from csv files.
///          All methods are only taylored for IO and this class is not
///          designed to serve as a data container. All entries are stored
///          as strings. The representation of the data is assumed and enforced
///          as a table with one header line.
///
/// \author  Alexandru Duliu
/// \date    Jul 9, 2014
///

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <iterator>


template <typename Ch>
class basic_csv
{
public:
    typedef std::basic_string<Ch> string_t;
    typedef std::vector<string_t > row_t;

public:
    basic_csv();
    basic_csv( const char separator );
    ~basic_csv();

    // options
    void reserve( size_t row_count );
    void clear();

    // header
    void set_header( const row_t& columns_names );
    const row_t& header() const;

    // row operations
    void add_row( const row_t& row );
    const row_t& row( size_t idx ) const; /// 0 is first row not the header
    const std::vector<row_t>& rows() const;

    // IO
    void load( const std::string& filename );
    void save( const std::string& filename ) const;

protected:
    row_t read_line( std::ifstream& file, size_t column_count );
    void write_line( std::ofstream& file, const row_t& line ) const;

protected:
    const char m_separator;
    row_t m_header;
    std::vector<row_t> m_rows;
};


/////
// Implementation
///

template <typename Ch>
inline basic_csv<Ch>::basic_csv() :
    m_separator(',')
{
}


template <typename Ch>
inline basic_csv<Ch>::basic_csv( const char separator ) :
    m_separator(separator)
{
}


template <typename Ch>
inline basic_csv<Ch>::~basic_csv()
{
    clear();
}


template <typename Ch>
inline void basic_csv<Ch>::reserve( size_t row_count )
{
    m_rows.reserve( row_count );
}


template <typename Ch>
inline void basic_csv<Ch>::clear( )
{
    // header
    m_header.clear();
    // individual rows
    for( row_t& row : m_rows )
        row.clear();
    // and the row container
    m_rows.clear();
}


template <typename Ch>
inline void basic_csv<Ch>::set_header( const row_t& columns_names )
{
    m_header = columns_names;
}


template <typename Ch>
inline const typename basic_csv<Ch>::row_t& basic_csv<Ch>::header() const
{
    return m_header;
}


template <typename Ch>
inline void basic_csv<Ch>::add_row( const row_t& row )
{
    if( row.size() != m_header.size() )
        throw std::runtime_error("basic_csv::load: row has different size than header ( " +
                                 std::to_string(m_rows.back().size()) + " != " + std::to_string(m_header.size()) + " ).");
    m_rows.push_back( row );
}


template <typename Ch>
inline const typename basic_csv<Ch>::row_t& basic_csv<Ch>::row( size_t idx ) const
{
    if( idx >= m_rows.size() )
        throw std::runtime_error("basic_csv::row: index (" + std::to_string(idx) + ") out of bounds.");
    return m_rows[idx];
}


template <typename Ch>
inline const std::vector<typename basic_csv<Ch>::row_t>& basic_csv<Ch>::rows() const
{
    return m_rows;
}


template <typename Ch>
inline void basic_csv<Ch>::load( const std::string& filename )
{
    // open the file
    std::string temp;
    std::ifstream file( filename.c_str(), std::ios::in|std::ios::binary);
    if( !file.is_open() )
        throw std::runtime_error( "basic_csv::load: could not open file \"" + filename + "\"." );

    // count rows (so basically line_count - 1)
    file.unsetf(std::ios_base::skipws); // prevent the skipping of new lines
    size_t row_count = std::count( std::istream_iterator<Ch>(file), std::istream_iterator<Ch>(), '\n');
    //file.setf(std::ios_base::skipws); // reenable line skipping
    file.clear(); // clear the EOF flag

    // counnt columns
    file.seekg( 0, std::ios::beg ); // bring "cursor" back to the beginning
    std::getline(file,temp);
    size_t column_count = (temp.size()>0) ? (std::count( temp.begin(), temp.end(), m_separator ) + 1 ) : 0;

    // read the header
    file.seekg( 0, std::ios::beg ); // bring "cursor" back to the beginning
    m_header = read_line(file, column_count);

    // read the rows
    reserve( row_count );
    for( size_t row=0; row<row_count; row++  )
    {
        m_rows.push_back( read_line( file, column_count ) );
        if( m_rows.back().size() != m_header.size() )
        {
            file.close();
            throw std::runtime_error( "basic_csv::load: row[" + std::to_string(row) + "] has different size than header ( " +
                                      std::to_string(m_rows.back().size()) + " != " + std::to_string(m_header.size()) + " ).");
        }
    }

    // Cave Johnson, we're done here
    file.close();
}


template <typename Ch>
inline void basic_csv<Ch>::save( const std::string& filename ) const
{
    // open the file
    std::ofstream file( filename.c_str(), std::ios::out|std::ios::binary);
    if( !file.is_open() )
        throw std::runtime_error( "basic_csv::save: could not open file \"" + filename + "\"." );

    // write header
    write_line( file, m_header );

    // write rows
    for( const row_t& row : m_rows )
        write_line( file, row );

    // Cave Johnson, we're done here too
    file.close();
}


template <typename Ch>
inline typename basic_csv<Ch>::row_t basic_csv<Ch>::read_line( std::ifstream& file, size_t column_count )
{
    // first make a copy of the complete line
    string_t line;
    std::getline( file, line );
    std::basic_stringstream<Ch> line_ss;
    line_ss << line;
    line_ss.seekg( 0, std::ios::beg );

    // now parse
    row_t row(column_count,string_t());
    for( size_t col=0; col<column_count; col++ )
        std::getline( line_ss, row[col], m_separator );
    return row;
}


template <typename Ch>
inline void basic_csv<Ch>::write_line( std::ofstream& file, const row_t& line ) const
{
    file << line[0];
    for( int i=1; i<line.size(); i++ )
        file << m_separator << line[i];
    file << "\n";
}


// one typedef to rule them all
typedef basic_csv<char> csv;
