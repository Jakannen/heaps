// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
            // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty pairing heap with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, numNodes(0) {
        // TODO: Implement this function.
    } // PairingPQ()
    

    // Description: Construct a pairing heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, numNodes(0) {
        // TODO: Implement this function.
        for (auto it = start; it != end; ++it) {
            push(*it);
        }
        
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, root(nullptr), numNodes(0) {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid Pairing Heap.

        std::deque<Node*> d;
        d.push_front(other.root);

        while (!d.empty()) {
           
            if (d.front()->child != nullptr) {
                d.push_back(d.front()->child);
            }
            if (d.front()->sibling != nullptr) {
                d.push_back(d.front()->sibling);
            }
            push(d.front()->elt);
            d.pop_front();
        }

       
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.

        // HINT: Use the copy-swap method from the "Arrays and Containers" lecture.

        PairingPQ r(rhs);
        std::swap(root, r.root);
        std::swap(numNodes, r.numNodes);

        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.

        if (numNodes == 0) {
            return;
        }

        std::deque<Node*> d;
        d.push_back(root);
        while (!d.empty()) {
            Node* n = d.front();
            if (n->sibling != nullptr) {
                d.push_back(n->sibling);
            }
            if (n->child != nullptr) {
                d.push_back(n->child);
            }
            n->parent = nullptr;
            n->sibling = nullptr;
            n->child = nullptr;
            delete n;
            d.pop_front();
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.

        std::deque<Node*> d;
        d.push_back(root);
        root = nullptr;
        while (!d.empty()) {
            Node* n = d.front();

            if (n->sibling != nullptr) {
                d.push_back(n->sibling);
                n->sibling = nullptr;
            }
            if (n->child != nullptr) {
                d.push_back(n->child);
                n->child = nullptr;
            }
            if (root != nullptr)
                root = meld(root, n);
            else {
                root = n;
            }
           
            d.pop_front();
            
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already done.
    //              You should implement push functionality entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the pairing heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the pairing heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        Node* new_root = root->child;
        delete root;
        
        if (new_root == nullptr) {
            numNodes = 0;
            root = nullptr;
            return;
        }

        if (new_root->sibling == nullptr) {
            --numNodes;
            root = new_root;
            return;

        }
        Node* t = new_root;
        std::deque<Node*> d;
        while (t != nullptr) {
            d.push_back(t);
            t = t->sibling;
        }
        

        while (d.size() > 1) {
            Node* first = d.front();
            d.pop_front();
            Node* second = d.front();
            d.pop_front();
            first->parent = nullptr;
            first->sibling = nullptr;
            second->parent = nullptr;
            second->sibling = nullptr;

            d.push_back(meld(first, second));
        }
        root = d.front();

        --numNodes;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function

        // These lines are present only so that this provided file compiles.
       
        return root->elt;      // TODO: Delete or change this line
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return numNodes; // TODO: Delete or change this line
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        return size() == 0; // TODO: Delete or change this line
    } // empty()


    // Description: Updates the priority of an element already in the pairing heap by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function

        node->elt = new_value;
        if (node->parent == nullptr) {
            return;
        }
        if (this->compare(node->elt, node->parent->elt)) {
            return;
        }
        //leftmost value
        if (node->parent->child == node) {
            node->parent->child = node->sibling;
            node->sibling = nullptr;
            node->parent = nullptr;

            root = meld(root, node);
        }
        // not
        else {
            Node* n = node->parent->child;
            while (n->sibling != node) {
                n = n->sibling;
            }
            n->sibling = node->sibling;
            node->sibling = nullptr;
            node->parent = nullptr;
            root = meld(root, node);
        }
        

    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val) {
        // TODO: Implement this function
        Node* n = new Node(val);
        ++numNodes;
        if (numNodes == 1) {
            root = n;
            return root;
        }
        root = meld(root, n);
        return n; // TODO: Delete or change this line
    } // addNode()

    

private:
    // TODO: Add any additional member variables or member functions you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    Node* root;
    uint32_t numNodes;
    
    // NOTE: For member variables, you are only allowed to add a "root pointer"
    //       and a "count" of the number of nodes.  Anything else (such as a deque)
    //       should be declared inside of member functions as needed.

    Node* meld(Node* r, Node* newNode) {
        if (this->compare(r->elt, newNode->elt)) {
            r->parent = newNode;
            if (newNode->child != nullptr) {
                r->sibling = newNode->child;
            }
            newNode->child = r;

            return newNode;
        }
        else {
            newNode->parent = r;
            if (r->child != nullptr) {
                newNode->sibling = r->child;
            }

            r->child = newNode;

            return r;
        }
    }
};


#endif // PAIRINGPQ_H
