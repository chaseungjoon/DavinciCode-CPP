# SEGFAULT

## After some cards are revealed, the prob vector become empty

* probs empty -> guessingAlgorithm cannot index into it -> segfault

### Functions that delete values from the prob vector

1. adjustProb : Observing surrounding cards, adjust the value vector of this card
2. updateProb : After ValVec is updated, removes values that is included in ValVec
3. deleteFromProb (called after wrong guess)