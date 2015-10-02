// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
// Copyright (C) 2009 Sebastian Redl
// Copyright (C) 2010 Eric Nodwell
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------
#ifndef BOOST_PROPERTY_TREE_CONF_PARSER_HPP_INCLUDED
#define BOOST_PROPERTY_TREE_CONF_PARSER_HPP_INCLUDED

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/detail/ptree_utils.hpp>
#include <boost/property_tree/detail/file_parser_error.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <locale>

namespace boost { namespace property_tree { namespace conf_parser
{

    /**
     * Determines whether the @c flags are valid for use with the conf_parser.
     * @param flags value to check for validity as flags to conf_parser.
     * @return true if the flags are valid, false otherwise.
     */
    inline bool validate_flags(int flags)
    {
        return flags == 0;
    }

    /** Indicates an error parsing CONF formatted data. */
    class conf_parser_error: public file_parser_error
    {
    public:
        /**
         * Construct an @c conf_parser_error
         * @param message Message describing the parser error.
         * @param filename The name of the file being parsed containing the
         *                 error.
         * @param line The line in the given file where an error was
         *             encountered.
         */
        conf_parser_error(const std::string &message,
                         const std::string &filename,
                         unsigned long line)
            : file_parser_error(message, filename, line)
        {
        }
    };

    /**
     * Read CONF from a the given stream and translate it to a property tree.
     * @note Clears existing contents of property tree. In case of error
     *       the property tree is not modified.
     * @throw conf_parser_error If a format violation is found.
     * @param stream Stream from which to read in the property tree.
     * @param[out] pt The property tree to populate.
     */
    template<class Ptree>
    void read_conf(std::basic_istream<
                    typename Ptree::key_type::value_type> &stream,
                  Ptree &pt)
    {
        typedef typename Ptree::key_type::value_type Ch;
        typedef std::basic_string<Ch> Str;
        const Ch pound = stream.widen('#');
        const Ch lbracket = stream.widen('[');
        const Ch rbracket = stream.widen(']');

        Ptree local;
        unsigned long line_no = 0;
        Ptree *section = 0;
        Str line;

        // For all lines
        while (stream.good())
        {

            // Get line from stream
            ++line_no;
            std::getline(stream, line);
            if (!stream.good() && !stream.eof())
                BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                    "read error", "", line_no));

            // If line is non-empty
            line = property_tree::detail::trim(line, stream.getloc());
            if (!line.empty())
            {
                // Comment, section or key?
                if (line[0] == pound)
                {
                    // Ignore comments
                }
                else if (line[0] == lbracket)
                {
                    // If the previous section was empty, drop it again.
                    if (section && section->empty())
                        local.pop_back();
                    typename Str::size_type end = line.find(rbracket);
                    if (end == Str::npos)
                        BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                            "unmatched '['", "", line_no));
                    Str key = property_tree::detail::trim(
                        line.substr(1, end - 1), stream.getloc());
                    if (local.find(key) != local.not_found())
                        BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                            "duplicate section name", "", line_no));
                    section = &local.push_back(
                        std::make_pair(key, Ptree()))->second;
                }
                else
                {
                    Ptree &container = section ? *section : local;
                    typename Str::size_type eqpos = line.find(Ch('='));
                    if (eqpos == Str::npos)
                        BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                            "'=' character not found in line", "", line_no));
                    if (eqpos == 0)
                        BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                            "key expected", "", line_no));
                    Str key = property_tree::detail::trim(
                        line.substr(0, eqpos), stream.getloc());
                    Str data = property_tree::detail::trim(
                        line.substr(eqpos + 1, Str::npos), stream.getloc());
                    if (container.find(key) != container.not_found())
                        BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                            "duplicate key name", "", line_no));
                    container.push_back(std::make_pair(key, Ptree(data)));
                }
            }
        }
        // If the last section was empty, drop it again.
        if (section && section->empty())
            local.pop_back();

        // Swap local ptree with result ptree
        pt.swap(local);

    }

    /**
     * Read CONF from a the given file and translate it to a property tree.
     * @note Clears existing contents of property tree.  In case of error the
     *       property tree unmodified.
     * @throw conf_parser_error In case of error deserializing the property tree.
     * @param filename Name of file from which to read in the property tree.
     * @param[out] pt The property tree to populate.
     * @param loc The locale to use when reading in the file contents.
     */
    template<class Ptree>
    void read_conf(const std::string &filename, 
                  Ptree &pt,
                  const std::locale &loc = std::locale())
    {
        std::basic_ifstream<typename Ptree::key_type::value_type>
            stream(filename.c_str());
        if (!stream)
            BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                "cannot open file", filename, 0));
        stream.imbue(loc);
        try {
            read_conf(stream, pt);
        }
        catch (conf_parser_error &e) {
            BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                e.message(), filename, e.line()));
        }
    }

    namespace detail
    {
        template<class Ptree>
        void check_dupes(const Ptree &pt)
        {
            if(pt.size() <= 1)
                return;
            const typename Ptree::key_type *lastkey = 0;
            typename Ptree::const_assoc_iterator it = pt.ordered_begin(),
                                                 end = pt.not_found();
            lastkey = &it->first;
            for(++it; it != end; ++it) {
                if(*lastkey == it->first)
                    BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                        "duplicate key", "", 0));
                lastkey = &it->first;
            }
        }
    }

    /**
     * Translates the property tree to CONF and writes it the given output
     * stream.
     * @pre @e pt cannot have keys both data and children.
     * @pre @e pt cannot be deeper than two levels.
     * @pre There cannot be duplicate keys on any given level of @e pt.
     * @throw conf_parser_error In case of error translating the property tree to
     *                         CONF or writing to the output stream.
     * @param stream The stream to which to write the CONF representation of the 
     *               property tree.
     * @param pt The property tree to tranlsate to CONF and output.
     * @param flags The flags to use when writing the CONF file.
     *              No flags are currently supported.
     */
    template<class Ptree>
    void write_conf(std::basic_ostream<
                       typename Ptree::key_type::value_type
                   > &stream,
                   const Ptree &pt,
                   int flags = 0)
    {
        using detail::check_dupes;

        typedef typename Ptree::key_type::value_type Ch;
        typedef std::basic_string<Ch> Str;

        BOOST_ASSERT(validate_flags(flags));
        (void)flags;

        if (!pt.data().empty())
            BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                "ptree has data on root", "", 0));
        check_dupes(pt);

        bool first_header = true;
        for (typename Ptree::const_iterator it = pt.begin(), end = pt.end();
             it != end; ++it)
        {
            check_dupes(it->second);
            if (it->second.empty()) {
                stream << it->first << Ch(' ') << Ch('=') << Ch(' ')
                    << it->second.template get_value<
                        std::basic_string<Ch> >()
                    << Ch('\n');
                first_header = false;
            } else {
                if (!it->second.data().empty())
                    BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                        "mixed data and children", "", 0));
                if (first_header) {
                    first_header = false;
                } else {
                    stream << Ch('\n');
                }
                stream << Ch('[') << it->first << Ch(']') << Ch('\n');
                for (typename Ptree::const_iterator it2 = it->second.begin(),
                         end2 = it->second.end(); it2 != end2; ++it2)
                {
                    if (!it2->second.empty())
                        BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                            "ptree is too deep", "", 0));
                    stream << it2->first << Ch(' ') << Ch('=') << Ch(' ')
                        << it2->second.template get_value<
                            std::basic_string<Ch> >()
                        << Ch('\n');
                }
            }
        }

    }

    /**
     * Translates the property tree to CONF and writes it the given file.
     * @pre @e pt cannot have keys both data and children.
     * @pre @e pt cannot be deeper than two levels.
     * @pre There cannot be duplicate keys on any given level of @e pt.
     * @throw info_parser_error In case of error translating the property tree
     *                          to CONF or writing to the file.
     * @param filename The name of the file to which to write the CONF
     *                 representation of the property tree.
     * @param pt The property tree to tranlsate to CONF and output.
     * @param flags The flags to use when writing the CONF file.
     *              The following flags are supported:
     * @li @c skip_conf_validity_check -- Skip check if ptree is a valid conf file. The
     *     validity check covers the preconditions but takes <tt>O(n log n)</tt>
     *     time.
     * @param loc The locale to use when writing the file.
     */
    template<class Ptree>
    void write_conf(const std::string &filename,
                   const Ptree &pt,
                   int flags = 0,
                   const std::locale &loc = std::locale())
    {
        std::basic_ofstream<typename Ptree::key_type::value_type>
            stream(filename.c_str());
        if (!stream)
            BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                "cannot open file", filename, 0));
        stream.imbue(loc);
        try {
            write_conf(stream, pt, flags);
        }
        catch (conf_parser_error &e) {
            BOOST_PROPERTY_TREE_THROW(conf_parser_error(
                e.message(), filename, e.line()));
        }
    }

} } }

namespace boost { namespace property_tree
{
    using conf_parser::conf_parser_error;
    using conf_parser::read_conf;
    using conf_parser::write_conf;
} }

#endif
