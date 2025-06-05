❗Player.cpp is currently work in progress

# Program Layout

---

## Card.h

Define card by "sequence"

Cards in Davinci Code follows a specific sequence layout;

(Black 0 -> White 0 -> Black 1 -> White 1 -> ... -> Black 11 -> White 11)

Therefore, each card's color can be defined as 

```cpp
(seq % 2) ? Black : White
```

and each card's number is 

```cpp
seq / 2
```

boolean attribute "shown" indicates if card is shown

## Player.h

There are only two instances of Player class;
* human
* computer

hand attribute is the card hand of either player
```cpp
std::vector<Card> hand 
```

For the computer's guessing algorithm, a ***probability vector*** exists as an attribute
```cpp
struct probData{
    std::string color;
    std::vector<int> values;
};
std::vector<probData> prob;
```
Indicating cards that has not been shown/known yet, a sequence vector exists as an attribute.
```cpp
std::vector<int> seqVec;
```

For the computer's guessing algorithm, the following functions are implemented
```cpp
void initializeProb(const std::vector<Card>& hand);
void updateProb(const std::vector<Card>& hand);
void propagation();
```

## Game.h

Takes care of initialization, printing card layouts and checking winning situations.

---

# AI Algorithm


## Algorithm Overview

1. Draw card

2. Update the probability vector of opponent's cards

3. Choose the highest probable guess (smallest probability vector)

4. Repeat until win
---
### Sequence Vector

Create a sequence vector that contains the sequence of cards that is ***shown*** at the moment
-> **non-candidate cards**


### Probability Vector

For the opponent's each concealed card, create probability data as below (***probData***)

```cpp
struct probData{
    std::string color;
    std::vector<int> values;
};
```

***color*** is the color of the card, values are ***what the card's numbers can be***. 

***values*** can only be picked if not included in the ***sequence vector***

For all the opponent's cards, Create a ***prob*** vector of these ***probData***.

---

## Initialize Probability Vector

### 1. Create probData for black cards

If the opponent has ***n*** black cards, the ***0th*** card's value can be **0,1, ..., 11-(n-1)**

with the same principle, the ***(n-1)th*** card's value can be **(n-1),n, ..., 11**

```aiignore
for N black cards, the ith (i<n) black card's value vector is;
{i, i+1, ..., 11-(n-1)}
```

### 2. Create probData for white cards

Firstly, create probData with the same principle as black cards
```aiignore
for N white cards, the ith (i<n) white card's value vector is;
{i, i+1, ..., 11-(n-1)}
```

if the number of black cards is greater than zero, we must put into account the value vector of **near black cards**.

for each white card, find ***left_closest_black*** and ***right_closest_black*** cards.

Delete values from the white card's values vector that is;
1) Smaller than ***min(left_closest_black.values)***
2) Larger than ***max(right_closest_black.values)***

If there are no ***left_closest_black*** or ***right_closest_black*** cards, ignore this process.

### 3. Fix probData for black cards

Same as what we did with the white cards, for black cards too;

if the number of white cards is greater than zero, we must put into account the value vector of **near white cards**.

for each black card, find ***left_closest_white*** and ***right_closest_white*** cards.

Delete values from the black card's values vector that is;
1) Smaller than ***min(left_closest_white.values)***
2) Larger than ***max(right_closest_white.values)***

If there are no ***left_closest_white*** or ***right_closest_white*** cards, ignore this process.

---

## Update Probability Vector

### 1. When the opponent's card gets revealed

For cards that is ```shown=true```, collapse the ```probData.values``` into the single value.

```cpp
probData.values = {3};
```

**{Propagation}**
Using the rules we implemented for initializing the probability vector, fix the neighboring ```probData.values``` vectors.

The collapsed probData being ```prob[n]```The order of fixing must be 
1. ```prob[n-1]``` and ```prob[n+1]```
2. ```prob[n-2]``` and ```prob[n+2]```
~

Just how a "propagation" would work.

### 2. When the opponent's hand gets added a card

Based on the left nearest card and right nearest card, create ```probData.values``` vector as follows;
```cpp
probData.values = {min(left_nearest_card)+1, ..., max(right_nearest_card)-1}
```

If the new card is the **leftmost/rightmost** card, put **min=0/max=11**

---
## Initial Guessing

* All guessings are done to cards that is ```shown=false```

In the ```prob``` vector, choose an index that has the smallest ```probData.values``` vector
→ ***Highest possibility of a correct guess***

Choose a random index of the ```probData.values``` vector, and submit the guess.

If the guess is correct, set the card ```shown=true```.

Then **update probability vector**.


---

## Additional Guessing

If the initial guessing is incorrect, additional guessing is not allowed.

For strategic reasons, additional guessing is only initiated when there is a ***definite-sized vector***
- definite-sized vector : ```sizeof(probData.values) <= 2```

The additional guessing is looped until;
1. Incorrect guess
2. No ***definite-sized vector*** available
