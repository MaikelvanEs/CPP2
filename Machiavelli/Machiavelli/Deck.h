#ifndef _DECK_H
#define _DECK_H
#include <vector>
#include <algorithm>

namespace cards {

	template<typename T>
	class Deck {
		// your containers here!
		std::vector<T> drawPile_;
		std::vector<T> discardPile_;
	public:
		Deck()
		{
		}

		Deck(std::vector<T> cards)
		{
			drawPile_ = cards;
		}

		void shuffle_stack()
		{
			merge_stacks();
			std::random_shuffle(drawPile_.begin(), drawPile_.end());
		}

		bool stack_empty() const
		{
			return drawPile_.empty();
		}

		bool discard_pile_empty() const
		{
			return discardPile_.empty();
		}

		T draw()
		{
			T card = drawPile_.begin();
			drawPile_.erase(drawPile_.begin());
			return card;
		}

		void discard(const T& card)
		{
			discardPile_.push_back(card);
		}
		void discard(T&&  card)
		{
			discardPile_.push_back(card);
		}

		void merge_stacks()
		{
			for (T card : discardPile_)
				drawPile_.push_back(card);
			discardPile_.clear();
		}
	}; // class template Deck<T>
} /// namespace cards
#endif
