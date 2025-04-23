/**
 * @file BItemVisitor.h
 * @brief Visitor pattern base class for BItem hierarchy.
 * @details Implements the Visitor design pattern for bencoded data structures.
 */
#ifndef BENCODING_BITEMVISITOR_H
#define BENCODING_BITEMVISITOR_H

namespace bencoding {

    class BDictionary;
    class BInteger;
    class BList;
    class BString;

    /**
     * @class BItemVisitor
     * @brief Abstract visitor for BItem hierarchy.
     *
     * Example visitor implementation:
     * @code
     * class MyVisitor : public BItemVisitor {
     * public:
     *     void visit(BDictionary *d) override { ... }
     *     void visit(BInteger *i) override { ... }
     *     // ... other visit methods
     * };
     * @endcode
     */
    class BItemVisitor {
    public:
        virtual ~BItemVisitor();

        /// @name Visit Methods
        /// @{
        virtual void visit(BDictionary* bDictionary) = 0;
        virtual void visit(BInteger* bInteger) = 0;
        virtual void visit(BList* blist) = 0;
        virtual void visit(BString* bString) = 0;
        /// @}

    protected:
        BItemVisitor();
    };

} // namespace bencoding
#endif