#ifndef _DECK_H
#define _DECK_H

namespace cards {

	template<typename T>
	class Deck {
		// your containers here!
	public:
		Deck() {
		}

		Deck(std::initializer_list<T> cards) {
		}

		template<typename Iter>
		Deck(Iter b, Iter e);

		void shuffle_stack() {
		}

		bool stack_empty() const {}
		bool discard_pile_empty() const {}

		T draw() {
		}

		void discard(const T& card) {
		}
		void discard(T&&  card) {
		}

		void push_bottom_stack(const T& card) {
		}
		void push_bottom_stack(T&& card) {
		}

		void push_top_stack(const T& card) {
		}
		void push_top_stack(T&& card) {
		}

		void merge_stacks(bool shuffle_afterwards = true) {
		}
	}; // class template Deck<T>
} /// namespace cards
#endif
