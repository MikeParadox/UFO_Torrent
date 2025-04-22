/**
 * @file prettyPrinter.h
 * @brief Pretty-printing for bencoded values.
 * @details Formats Value objects for human-readable display.
 */
#ifndef PRETTYPRINTER_H
#define PRETTYPRINTER_H

#include "valueTypes.h"
#include <boost/variant/apply_visitor.hpp>

namespace bencode {

    /**
     * @class PrettyPrinter
     * @brief Visitor for pretty-printing Value objects.
     */
    class PrettyPrinter : public boost::static_visitor<>
    {
    public:
        /**
         * @brief Constructs PrettyPrinter.
         * @param indentation Initial indentation level.
         */
        PrettyPrinter(int indentation = 0);

        /**
         * @brief Gets current indentation.
         * @return Indentation level.
         */
        int getIndentation() const;

        void operator()(int t) const;
        void operator()(const std::string& t) const;
        void operator()(const ValueDictionary& t) const;
        void operator()(const ValueVector& t) const;

    private:
        std::string getSpace() const;
        bool isAscii(int c) const;
        bool isAscii(const std::string& value) const;

    private:
        int indentation_; /**< Current indentation level. */
    };
};

#endif