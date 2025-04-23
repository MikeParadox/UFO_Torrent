/**
 * @file PrettyPrinter.h
 * @brief Pretty printer for BItem hierarchy.
 * @details Formats bencoded data in human-readable form with indentation.
 */
#ifndef BENCODING_PRETTYPRINTER_H
#define BENCODING_PRETTYPRINTER_H

#include <memory>
#include <string>
#include "BItemVisitor.h"

namespace bencoding {

    class BItem;

    /**
     * @class PrettyPrinter
     * @brief Formats bencoded data for human reading.
     *
     * Example usage:
     * @code
     * auto printer = PrettyPrinter::create();
     * std::cout << printer->getPrettyRepr(item);
     * @endcode
     */
    class PrettyPrinter : private BItemVisitor {
    public:
        /**
         * @brief Creates a new pretty printer
         * @return Unique pointer to new printer
         */
        static std::unique_ptr<PrettyPrinter> create();

        /**
         * @brief Formats item as human-readable string
         * @param data Item to format
         * @param indent Indentation string (default "    ")
         * @return Formatted string
         */
        std::string getPrettyRepr(std::shared_ptr<BItem> data,
            const std::string& indent = "    ");

    private:
        PrettyPrinter();

        /// @name Visitor Interface
        /// @{
        virtual void visit(BDictionary* bDictionary) override;
        virtual void visit(BInteger* bInteger) override;
        virtual void visit(BList* bList) override;
        virtual void visit(BString* bString) override;
        /// @}

        /// @name Indentation Management
        /// @{
        void storeCurrentIndent();
        void increaseIndentLevel();
        void decreaseIndentLevel();
        /// @}

    private:
        std::string prettyRepr;     ///< Accumulated output
        std::string indentLevel;    ///< Single indentation unit
        std::string currentIndent;  ///< Current indentation
    };

    /// @name Convenience Functions
    /// @{
    std::string getPrettyRepr(std::shared_ptr<BItem> data,
        const std::string& indent = "    ");
    /// @}

} // namespace bencoding
#endif