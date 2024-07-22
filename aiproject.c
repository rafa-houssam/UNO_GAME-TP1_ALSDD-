




 //-------------------------------------this part is used to include the necessary libraries in our tp-------------------------------
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<strings.h>
#include<windows.h>
#define NUM_START_CARDS 7
#define DELAY_MS 1 //  change the delay between characters
#define NUM_COLORS 4  //number of colors
#define NUM_NUMBERS 10 // number of numbers(we have 10 numbers from 0 to 9
#define NUM_ACTION_CARDS 3//number of action cards
#define NUM_SPECIAL_CARDS 2//number of special cards
#include <unistd.h>
#include <conio.h>
#include <pthread.h>
#include <time.h>
//---------------------------------------------------------------------------------------




//                                                          ********************************************************
//                                                         **********************************************************
//---------------------------------------------------------*                  Main data structure                   *---------------------------------------------------
//                                                         **********************************************************
//                                                          ********************************************************


typedef struct card { // a card's node implementation
    int number;// represents the number of the card between 0 and 9
    char color[10];// represents the color of the card (yellow,green,red or blue)
    char property[20];// this field is  used to specify if the card is +4,+2,color choice card,pass or reverse card
    struct card *next;// a pointer to the next card
} card;
//---------------------------------------------------------------------------------------
card *discard_stack = NULL;// initialization of the discared stack which contain nothing before starting the game
//---------------------------------------------------------------------------------------
typedef struct player{// a player's node implementation
card *cards;// a pointer to a card
struct player* next;// a pointer to the next player
struct player* prev;// a pointer to the previous player
char  id[20];// this field will specify if the player is a human or a robot(computer) ,if a player is declared as a robot he will play automaticly
char name[20];// contains the name of the player
}player;
//---------------------------------------------------------------------------------------


//                                                          ********************************************************
//                                                         **********************************************************
//---------------------------------------------------------*            abstract machine for player                 *---------------------------------------------------
//                                                         **********************************************************
//                                                          ********************************************************

//-------------------------------------//
player * next_player(player*ptr)// this function return a pointer to the next player
{
    return ptr->next;
}

//-------------------------------------//
player * prev_player(player*ptr)// this function return a pointer to the previous player
{
    return ptr->prev;
}

//-------------------------------------//
card * card_player(player*ptr)// this function return a pointer to the first card of the player
{
    return ptr->cards;
}
//--------------------------------------------------------------------------------

void allocate_player(player** head_player) {// this function will allocate an area in the memory for one player
    *head_player = malloc(sizeof(player));// allocation of the memory
    if (*head_player == NULL) {
        // Failed to allocate memory, handle the error
        printf("Failed to allocate memory.\n");
    }

    // Allocation successful, initialize the node(here we initialize all the fields in NULL)
    (*head_player)->cards = NULL;
    (*head_player)->next = NULL;
    (*head_player)->prev = NULL;

}

//----------------------------------------------------------------------------//
void ass_name(player **pointer, char name[20]) {// this function put a name in the name field of a player
    strcpy((*pointer)->name, name);
}
//----------------------------------------------------------------------------//
void link_players(player **ptr1,player **ptr2)//// this function will link two players by considering that the list is doubly linked
{
 (*ptr1)->next=*ptr2;// the first link
 (*ptr2)->prev=*ptr1;// the second link
}
//-------------------------------------------------------------------------------------------
char* id_player(player*plr)// this function return the id of a player(human or robot)
{
    return (plr->id);
}
//-------------------------------------------------------------------------------------------
 char*  name_player(player*plr)// this function return the name of the player
{
    return (plr->name);
}
//-------------------------------------------------------------------------------------------


void display_players(player *head_player) {//// this function displays all the participed players in the game
    player *current_player = head_player;
    if (head_player == NULL) {// no players found
        printf("No players to display.\n");
        return;
    }
//else we display the players
    printf("Players:\n");
    do {
        printf("+---------------------+\n");
        printf("|       %s            |\n", name_player(current_player));
        printf("+---------------------+\n");
        current_player = next_player(current_player);// move to the next player
    } while (current_player != head_player);// because the list is circular
}
//-------------------------------------------------------------------------------------------
void  free_players(player** plr) // freeing player's node
{
    free(*plr);
}

//-------------------------------------------------------------------------------------------




//                                                          ********************************************************
//                                                         **********************************************************
//---------------------------------------------------------*                abstract machine for card               *---------------------------------------------------
//                                                         **********************************************************
//                                                          ********************************************************


//---------------------------------------------------------------------------------------
card *next_card(card *ptr) {// this function return a pointer to the next card
    return ptr->next;
}
//--------------------------------------------------------------------------------//
void allocate_card(card **head_card) {// this function will allocate an area in the memory for one card
     *head_card = malloc(sizeof(card));// allocation of the memory
    if (*head_card == NULL) {
        // Failed to allocate memory, handle the error
        printf("Failed to allocate memory.\n");
    }

    // Allocation successful, initialize the node
    (*head_card)->next = NULL;// ininitially we put "NULL" in the next field of the card
}
//--------------------------------------------------------------------------------//
void assign_properties(card **pointer, int number, const char *color, const char *property) {// this function will assign the proprties
                                                                                               //(color,number,property)   for one card
    (*pointer)->number = number;//put the number
    strcpy((*pointer)->color, color);// put the color
    strcpy((*pointer)->property, property);// put the property
}
//--------------------------------------------------------------------------------//
void link_cards(card **ptr1, card **ptr2) {// link two cards
    (*ptr1)->next = *ptr2;
}
//---------------------------------------------------------------------------------//
void free_card(card** ptr){// free a pointer to a card
   free(*ptr) ;
}
//--------------------------------------------------------//
int number_card(card*current_card)// function which return the number of a card
{
    return (current_card->number);
}
//------------------------------------------------------------//
char* property_card(card * current_card)// function which return the property of a card(reverse,+2,+4,pass...)
{
    return (current_card->property);
}
//-------------------------------------------------------------//
char* color_card(card * current_card)// function which return the color of a card
{
    return (current_card->color);
}
//-------------------------------------------------------------//

void remove_card(card ** head_card,int *n){// remove one card ( n is represents the position)
  if ((*head_card) == NULL) {
        // Empty list of card , nothing to remove
        return;
    }

    card *ptr = *head_card;// to reserve the value of head card we put  it in another pointer
    if (*n == 1) {// in the case  od removing the first card
        *(head_card) =next_card(ptr) ;// updating the head pointer
    } else {
        int i = 1;// we use this variable to reach to needed position
        while (ptr != NULL && i < *n - 1) {//searching the card  to remove it
            ptr =next_card(ptr);
            i++;
        }
        if (ptr == NULL || next_card(ptr) == NULL) {
            // Node to be removed is beyond the end of the list
            return;
        }
        card *temp =next_card(ptr);
        ptr->next = next_card(temp);
        // int this 2 last instruction we link the previous card of the removed card to the next card of the removed card

    // remark:
    // in "remove card "function we don't free the memory for the removed card because we need to link it to the discared stack
    }
}
//************************************************************************************************************************************************





//                                                          ********************************************************
//                                                         **********************************************************
//---------------------------------------------------------*               initialization functions                 *-----------------------------
//                                                         **********************************************************
//                                                          ********************************************************

//----------------------------------------------------------------------------//
// Function to create a new Uno card

card *create_card(int number, const char *color, const char *property) {// function to create a card
    card *new_card;// declare a new pointer
    allocate_card(&new_card);//allocate an area in the memory for card
    if (new_card != NULL) {// that' mean there is no problem in the allocation process
        assign_properties(&new_card, number, color, property);// put the number,color and property to the created card
    }
    return new_card;// a pointer to the creatted card
}
//----------------------------------------------------------------------------//
//--------------------------------------2-deck creation--------------------------------------------
// Function to generate a standard Uno card deck

card *generate_deck() { // this function is used to create a deck  initially with 54 cards

    card *deck = NULL;// initialization of the deck
    const char *colors[NUM_COLORS] = {"Red", "Green", "Blue", "Yellow"};// an array which contain the possible colors
    const char *numbers[NUM_NUMBERS] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};// an array which contain the possible numbers
    const char *action_cards[NUM_ACTION_CARDS] = {"Reverse", "Pass", "+2"};// an array which action cards
    const char *special_cards[NUM_SPECIAL_CARDS] = {"+4", "Color Choice"};// an array which contain special cards

    // Add special cards
    for (int i = 0; i < NUM_SPECIAL_CARDS; i++) {
        card *current_card = create_card(-1, "Special", special_cards[i]);// here we choose the value -1 (in this case the number of the card is not important)
        link_cards(&current_card, &deck);// link the current card to the deck
        deck = current_card;// update the deck
    }

    // Add action cards
    for (int i = 0; i < NUM_COLORS; i++) {
        for (int j = 0; j < NUM_ACTION_CARDS; j++) {
            card *current_card = create_card(-1, colors[i], action_cards[j]);// create new card
            link_cards(&current_card, &deck);// link the current card to the deck
            deck = current_card;// update the deck
        }
    }

    // Add numbered cards
    for (int i = 0; i < NUM_COLORS; i++) {
        for (int j = 0; j < NUM_NUMBERS; j++) {
            card *current_card = create_card(j, colors[i], "Number");// create new card
            link_cards(&current_card, &deck);// link the current card to the deck
            deck = current_card;// update the deck
        }
    }

    return deck; // return the deck pointer
}
//--------------------------------------------------------------------//
// Function to display the properties of a card
void display_card(card *card) { // to dispay a card
    // this part is used to display the cards with real colors
    const char *color_code = "";// initialize the color code variable into ""

    // Determine color code based on the card's color

    if (strcmp(card->color, "Red") == 0) {
        color_code = "\033[91m"; // Red color
    } else if (strcmp(color_card(card), "Blue") == 0) {
        color_code = "\033[94m"; // Blue color
    } else if (strcmp(color_card(card), "Green") == 0) {
        color_code = "\033[92m"; // Green color
    } else if (strcmp(color_card(card), "Yellow") == 0) {
        color_code = "\033[93m"; // Yellow color
    }

    // Print the colored border and card informationr
     // here we use a colored rectangle and we display inside it the informations of the card
     printf("------------------------------------------------------------------------------------------------------------\n");
    printf("      %s+-------------------------------------------------------------------+ \n", color_code);
    printf("        | Color: %-6s |  | Number: %-5d | property: %-5s                    | \n", color_card(card), number_card(card),property_card(card) );
    printf("        +-------------------------------------------------------------------+ \n\033[0m");
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("\n");
}

//--------------------------------------------------------------------//
// Function to display all cards in the deck
void display_deck(card *deck) {
    card *current_card = deck;// to reserve the value of the deck pointer
    printf("Uno Deck:\n");
    while (current_card != NULL) {// to navigate through all the cards in the deck
        display_card(current_card);//display the card
        current_card = next_card(current_card);//move to the next card
    }
}
//--------------------------------------------------------------------//
// -----------------------------------card distribution-----------------------------------------------------------------------
// Function to shuffle the Uno deck
void shuffle_deck(card **deck) {// this function is used to shuffle the deck in the begining of the game
    card *current_card = *deck;
    int count = 0;// this variable is used to count the number of the cards in the deck
    while (current_card != NULL) {// iterate until the last card
        count++;
        current_card = next_card(current_card);// move to the next card
    }

    // Put all cards into a dynamic array
    card **cards_array = malloc(count * sizeof(card *));// create the dynamic array
    current_card = *deck; // the first card in the deck
    for (int i = 0; i < count; i++) {
        cards_array[i] = current_card;//assign the current card to an array field
        current_card = next_card(current_card);//move to the next card
    }

    // Shuffle the array
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);//here we choose a random value to garentee that the cards will be shuffled in a well way
        card *temp = cards_array[i];// we use another pointer to save cards_array value
        cards_array[i] = cards_array[j];//shuffling
        cards_array[j] = temp;// cards_array contains its real value after the shuffling process
    }

    // Reconstruct the deck
    *deck = NULL;
    for (int i = 0; i < count; i++) {  //  in this part we put all the cards in the deck
        cards_array[i]->next = *deck;
        *deck = cards_array[i];
    }

    free(cards_array);// free the created array to provide the memory for another operations
}
//------------------------------------------------//
void reverse_direction(player **current_player, int *direction) {// this function is used when a player play a reverse card
    // Toggle the direction of play
    *direction *= -1;// here we just multiply by -1 ( the value of direction will contain only 1 or -1)
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

void start_game(card **deck, card **discard_stack) {// we use this function to put the first card in the discared stack
    // Draw a card from the deck
    if (*deck == NULL) {
        printf("Error: Deck is empty!\n");
        return;
    }

    // Take the top card from the deck
    card *top_card = *deck;
    *deck = next_card(*deck);//update the deck

    // Add the top card to the discard stack
    top_card->next = *discard_stack;
    *discard_stack = top_card;

}

//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

// Function to reshuffle the discard stack into the deck

void reshuffle_discard_stack(card **discard_stack, card **deck) {// we use this function if the deck become empty
                                                                //after playing
    // Attach the discard stak to the end of the deck
    card *last_card = *deck;
    if (last_card != NULL) {
        while (next_card(last_card) != NULL) {
            last_card = next_card(last_card);//move to the next card
        }
        last_card->next = *discard_stack;
    } else {
        *deck = *discard_stack;// update the deck
    }
    // Reset the discard stack
    *discard_stack = NULL;

    // Shuffle the combined deck
    shuffle_deck(deck);//shuffling the deck
    start_game(deck,discard_stack);// put a card in the top of the  discared stack
}



//----------------------------------------------------------------------------//

// Function to deal cards to players
void deal_cards(player *players, card **deck,card**discard_stack) {// this function give to all players 7 cards in the begining of the game
    card *current_card = *deck;// initialize current _card pointer to the top of the deck
    player *current_player = players;// initialize current _player pointer to the first player


        for (int j = 0; j < NUM_START_CARDS; j++) {
            if (current_card == NULL) {
        printf("Deck is empty. Reshuffling discard stack into the deck...\n");
        reshuffle_discard_stack(discard_stack, deck);//reshuffling the deck
        current_card = *deck;
    }
            card *temp = current_card;
            current_card = next_card(current_card);//move to the next card
            temp->next = card_player(current_player);// deal cards to the player
            current_player->cards = temp;// updating the cards pointer of the current player
            if(j==0)// the last card will contain NULL in its next field
                temp->next=NULL;
        }
    *deck = current_card;// after dealing cards we update the deck value
}
//----------------------------------------------------------------------------//

// Special action card function: Draw cards for the next player
void draw_cards(player **current_player, int num_cards, card **deck,card**discard_stack) {// we use it if a player play +2 or +4 card

    for (int i = 1; i <= num_cards; i++) {
        // Check if the deck is empty
        if (*deck == NULL) {
            printf("The deck is empty. Reshuffling the discard stack.\n");
            // Reshuffle the discard stack into the deck
            reshuffle_discard_stack(discard_stack, deck);
        }

        // Draw a card from the deck and give it to the current player
        card *temp = *deck; // to reserve the value of the deck
        *deck = next_card(*deck);// move to the next card in the deck
        temp->next =  card_player((*current_player));//deal one  card to the player
       (*current_player)->cards = temp;// updating cards pointer of the current player
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

//function which calculate the number of cards of a player :
int count_player_cards(player *current_player) {
    int count = 0;//will contain the number of cards
    card *current_card = card_player(current_player);// to point to the first card of the current player
    while (current_card != NULL) {// until we reach the last card
        count++;
        current_card = next_card(current_card);// moving to the next card
    }
    return count;// return the number of cards
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

// Function to display player's cards with colored text

void display_player_cards(player *current_player) {
    int nbr_cards;// number of cards
    nbr_cards=count_player_cards(current_player);// nbr_cards is a variable which contain the number of cards of the current player
    int i=1;
    printf("Player: %s\n", name_player(current_player));// printing the name of the player
    printf("Cards:\n");
    card *current_card = card_player(current_player);// he we point to the first card of the current player
    while (i<=nbr_cards) {
        printf("the card number %d\n",i);
        display_card(current_card);// display a card
        printf("\n");
        sleep(0.5);// the time between the displaying  two succesive cards (0.5 seconds)
        current_card = next_card(current_card);// move to the next card
        i++;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

void printMenu(int choice, int num_choices, char *choices[]) {// print the menu for the user
    printf("\n");
    printf("\n");
    printf("you should select by using ""|up|"" and  ""|down|"" keys \n");
    printf("______________________________________________________\n");
     printf("\n");
     printf("\n");
     printf("\n");
     printf("\n");
     printf("\n");
     printf("\n");
     printf("\n");
     printf("                                                       *******************************                             \n");
     printf("******************************************************            Main Menu            *************************************************************************************\n");
     printf("                                                       *******************************                             \n");
     printf("\n");
     printf("\n");
     printf("\n");
    for (int i = 0; i < num_choices; i++) { // num_ choices means the number of choices available in the menu
        if (i == choice) {
            printf("-------|> ");
            printf("\033[1;32m"); // Set text color to green for highlighted option
        } else {
            printf("   ");
        }
        printf("%s\n", choices[i]);
        printf("\033[0m"); // Reset text color to default
        printf("\n");
    }
}
void printMenu2(int choice, int num_choices, char *choices[]) { //this function shows another menu ( it is used to show (robot or human))
    printf("\n");

    for (int i = 0; i < num_choices; i++) {
        if (i == choice) {
            printf("-------> ");
            printf("\033[1;32m"); // Set text color to green for highlighted option
        } else {
            printf("   ");
        }
        printf("%s\n", choices[i]);
        printf("\033[0m"); // Reset text color to default
        printf("\n");
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//
void initialize_players(player **head_player, int nbr_players, card **deck, card **discard_stack) {// this function is used to give the players their cards
                                                                                                  // and their names and thier ids
    char name[20];
    player *prev_player = NULL;
    player *current_player = NULL;

    for (int i = 0; i < nbr_players; i++) {// to iterate through all players
        player *new_player ;
        allocate_player(&new_player);// add new player
        if (new_player == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }

        printf("Enter name for player %d: ", i + 1);
        scanf("%19s", name);  // Read player name


        ass_name(&new_player, name);
        // Assign name to player

        //------------------ this part is used for the menu-----------------------------------
        int choice = 0;
    int num_choices = 2;// number of choices in this menu
    char *choices[] = {
        "human",// if a player is a human
        "robot",// if a player is robot
    };

    while (1) {

            system("cls");// clear the screen
      printf("is %s human or robot:  \n",name_player(new_player));

        // Print menu with options and highlight the selected one
        printMenu2(choice, num_choices, choices);



         // this part is used to implement the selection of a choice  by "up" and "down" arrows from the keyboard



        // Capture arrow key presses for navigation
        int key = getch();
        if (key == 224) { // Arrow key detected
            key = getch(); // Read the next character to determine the arrow key
            if (key == 72 && choice > 0) { // Up arrow
                choice--;
            } else if (key == 80 && choice < num_choices - 1) { // Down arrow
                choice++;
            }
        } else if (key == '\r') { // Enter key
            // Perform action based on selected option
            if (choice == 0) {
             strcpy(new_player->id,"human");
               break; // End

            } else  {
             strcpy(new_player->id,"robot");
               break; // End

            }
        }
    }

        deal_cards(new_player, deck,discard_stack);// deal card to the current player

        if (*head_player == NULL) {  // in the case of  First player
            *head_player = new_player;
            current_player = new_player;
        } else {
            link_players(&prev_player, &new_player);  // Link previous player to current player
            link_players(&current_player, &new_player);  // Link current player to next player
            current_player = new_player;
        }
        prev_player = new_player;  // Update prev_player for next iteration
    }

    // Make the list circular

    link_players(&current_player, head_player);  // Link last player to first player
}




///--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

int is_winner(player *head_player) {// this function determines if there exist a winner or not yet
    player *current_player = head_player;// current player pointer will point to the first player
    do {
        // Check if the current player has no cards left
        if (card_player(current_player) == NULL) {
            // Player has no cards left, so they are the winner
            return 1; // Return true
        }
        current_player = next_player(current_player);// move to the next player
    } while (current_player != head_player);// bacause th elist is circular

    // No player has won yet
    return 0; // Return false
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

int can_play_card(player *current_player, card *discard_stack, card **deck) {// this function is used to know if a player can play a card or no
    // Check if the player has any cards
    if (card_player(current_player) == NULL) {
        printf("Player %s has no cards left.\n",name_player(current_player));
        return 0; // Player has no cards to play

    }
    // here we check if the top card of discard stack is special
  else if (strcmp(property_card(discard_stack), "Reverse") == 0 ||
            strcmp(property_card(discard_stack), "Pass") == 0 ||
            strcmp(property_card(discard_stack), "+2") == 0 ||
            strcmp(property_card(discard_stack), "+4") == 0 ||
            strcmp(property_card(discard_stack), "Color Choice") == 0) {
            return 1; // Player can play any card because the discared stack is playable

            }
    // Check if any card in the player's hand can be played without using the discard stack
        card *current_card = card_player(current_player) ;
    while (current_card != NULL) {
        if (strcmp(property_card(current_card), "Reverse") == 0 ||
            strcmp(property_card(current_card), "Pass") == 0 ||
            strcmp(property_card(current_card), "+2") == 0 ||
            strcmp(property_card(current_card), "+4") == 0 ||
            strcmp(property_card(current_card), "Color Choice") == 0) {
            return 1; // Player can play a special action card

        }
        current_card = next_card(current_card);// move to the next card
    }

    // Check if any card in the player's hand can be played by matching the color or number of the top card on the discard stack
    current_card = card_player(current_player);
    while (current_card != NULL) {
        if (strcmp(color_card(current_card), color_card(discard_stack)) == 0 ||
           number_card(current_card)  == number_card(discard_stack) ) {
            return 1; // Player can play a regular card
        }
        current_card = next_card(current_card);
    }

    // If the player cannot play any card, draw a card from the deck
    printf(" %s cannot play any card. Drawing a card from the deck...\n",name_player(current_player));
    sleep(2);// stop for 2 seconds

    draw_cards(&current_player, 1, deck,&discard_stack);// drawing 1 card from the deck
    // here it is a repeated part of the previous part to see if the player can play a card after drawing card from the deck
     current_card = card_player(current_player);

    // Check if any card in the player's hand can be played without using the discard stack
    while (current_card != NULL) {
        if (strcmp(property_card(current_card), "Reverse") == 0 ||
            strcmp(property_card(current_card), "Pass") == 0 ||
            strcmp(property_card(current_card), "+2") == 0 ||
            strcmp(property_card(current_card), "+4") == 0 ||
            strcmp(property_card(current_card), "Color Choice") == 0) {
            return 1; // Player can play a special action card

        }
        current_card = next_card(current_card);
    }

    // Check if any card in the player's hand can be played by matching the color or number of the top card on the discard stack
    current_card = card_player(current_player);
    while (current_card != NULL) {
        if (strcmp(color_card(current_card), color_card(discard_stack)) == 0 ||
           number_card( current_card) == number_card(discard_stack) ) {
            return 1; // Player can play a regular card
        }
        current_card = next_card(current_card);
    }

}
//---------------------------------------------------------------------//




//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

void play_card(player **current_player, card **discard_stack, card **deck, int *direction) {// this function make all necessary action to make
                                                                                            // a player play a card if he can
     player*ptr1;
    // Check if the player can play any card

    if (can_play_card(*current_player, *discard_stack, deck)==0) {
        printf("      %s cannot play any card.\n", name_player(*current_player));
        sleep(2);
        if((is_winner(*current_player)==0)){// Move to the next player if ther is no winner
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);// if direction=1 we move to the next player
        } else {// else we move to the previous player
            *current_player = prev_player(*current_player);
        }
}
        return;
    }

    // Display player's cards
    display_player_cards(*current_player);
    //-------------------------------
    // Display the top card on the discard stack
        printf("\n--------------------------------Top card on the discard stack:---------------------------------------\n");
        printf("             ");
        display_card(*discard_stack);
        printf("\n");
        printf("\n");


    // Calculate the number of cards in the player's hand
    int num_cards = count_player_cards(*current_player);

    // Prompt the player to select a card to play
    int card_index;
    int j=1;
    debut:// we use this label if the player is robot to garentee that the robot will play a suitable card
    if(strcmp(id_player(*current_player),"human")==0)// if the player is  human
    {
     do {
        printf("Enter the number of the card you want to play (1-%d): ", num_cards);
        scanf("%d", &card_index);
        if (card_index < 1 || card_index > num_cards)
            printf("    please enter a valid number\n");
    } while (card_index < 1 || card_index > num_cards);

    }
   else{ // if the player is robot we choose a random card but after that we will garentee that the robot will play a suitable card

        card_index= rand() %(num_cards) + 1;
        if(j==1){printf("%s is thinking...\n",name_player(*current_player));// just to show that the computer is choosing a card
        sleep(1.5);}

        if(can_play_card(*current_player,*discard_stack,deck)==1)//if there exist a playable cards
            {
                if(j==1) {  printf("%s is choosing a suitable card ...\n",name_player(*current_player));
        sleep(1);}}

        else{
            printf("%s can't play any card ...\n ",name_player(*current_player));
            sleep(2);// stop 2 seconds
            if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player =next_player((*current_player)) ;
        } else {
            *current_player =prev_player((*current_player));
        }
}

        }

    }

   // Find the selected card in the player's hand if the player is human
    card *selected_card = card_player(*current_player);
    for (int i = 1; i < card_index ; i++) {
        selected_card = next_card(selected_card);// searching the selected card
    }


    // Check if the selected card can be played

    if (strcmp( property_card(selected_card), "Reverse") == 0) {// the played card is the reverse card
        // Reverse the direction of play
        reverse_direction(current_player, direction);// we reverse the direction
        // Remove the selected card from the player's hand
       remove_card(&((*current_player)->cards),&card_index);
           //Move the selected card to the top of the discard stack
            selected_card->next = *discard_stack;
            *discard_stack = selected_card;


        printf("              %s played the Reverse card.\n", name_player(*current_player));
        printf(" _________________________________________________________________________________\n");
        printf("                  the direction of playing is reversed                            \n");
        printf("___________________________________________________________________________________\n");
        sleep(2); //to delay 2 seconds

        if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);
        } else {
            *current_player = prev_player(*current_player);
        }
}


    } else if (strcmp(property_card(selected_card), "Pass") == 0) {// the played card is the pass card
        // Remove the selected card from the player's hand
         remove_card(&((*current_player)->cards),&card_index);
           //Move the selected card to the top of the discard stack
            selected_card->next = *discard_stack;
            *discard_stack = selected_card;
        printf("              %s played the Pass card.\n", name_player(*current_player));
         printf(" _________________________________________________________________________________\n");
        printf("                  the next player will not play his role                           \n");
        printf("___________________________________________________________________________________\n");
        sleep(2); //to delay 2 seconds

        if((is_winner(*current_player)==0)){// Move to the next of the next  player or the the previous of the previous player
        if ((*direction) == 1) {
            *current_player =next_player(next_player(*current_player)) ;// move to the next of the next player
        } else {
            *current_player =prev_player(prev_player(*current_player));// move to the previous of the previous player
        }
}

    } else if (strcmp(property_card(selected_card), "+2") == 0) {// the current player play +2 card
             // here we point to the next or the previous player depending on the direction
               if ((*direction) == 1) {
             ptr1=next_player(*current_player);
             }
             else{
                ptr1=prev_player(*current_player);
                           }
        // Draw two cards for the next player and skip their turn
        draw_cards(&(ptr1), 2, deck,discard_stack);
        // Remove the selected card from the player's hand
          remove_card(&((*current_player)->cards),&card_index);
           //Move the selected card to the top of the discard stack
            selected_card->next = *discard_stack;
            *discard_stack = selected_card;
        printf("            %s played the +2 card.\n", name_player(*current_player));
         printf(" _________________________________________________________________________________\n");
        printf("                  the next player will draw 2 cards from the deck                  \n");
        printf("___________________________________________________________________________________\n");
        sleep(2); //to delay 2 seconds
        if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);
        } else {
            *current_player = prev_player(*current_player);
        }
}
    } else if (strcmp(property_card(selected_card), "+4") == 0) {// the current player play +4 card

        // Draw four cards for the next player and skip their turn
        if ((*direction) == 1) {
             ptr1=next_player(*current_player);// ptr1 will point to the next player if the direction is 1
             }
             else{ //else ptr1 will point to the previous player if (if the direction is -1)
                           ptr1=prev_player(*current_player);
                           }
        draw_cards(&(ptr1),4, deck,discard_stack);// draw 4 cards for the next player
         // Remove the selected card from the player's hand
           remove_card(&((*current_player)->cards),&card_index);
           //Move the selected card to the top of the discard stack
            selected_card->next = *discard_stack;
            *discard_stack = selected_card;
        printf("          %s played the +4 card.\n", name_player(*current_player));
         printf(" _________________________________________________________________________________\n");
        printf("                  the next player will draw 4 cards from the deck                   \n");
        printf("___________________________________________________________________________________\n");
        sleep(2); //to delay 2 seconds
        if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);
        } else {
            *current_player = prev_player(*current_player);
        }
}

    } else if (strcmp(property_card(selected_card), "Color Choice") == 0) {//  if the player plays a color choice card
        // Allow the player to choose a new color
        char new_color[10];
        printf("          %s played the Color Choice card.\n", name_player(*current_player));
         if(strcmp(id_player(*current_player),"human")==0){ do{ // if the player is human he must select a color
            printf("Enter the new color (Red, Green, Blue, or Yellow): ");
            scanf("%9s", &new_color);
            if(strcmp(new_color, "Red")!=0 && strcmp(new_color, "Green")!=0 &&
               strcmp(new_color, "Blue")!=0 && strcmp(new_color, "Yellow")!=0)
            printf("Oups!!!! invalide choice please respect lower case and upercase\n");

        }while(strcmp(new_color, "Red")!=0 && strcmp(new_color, "Green")!=0 &&
                strcmp(new_color, "Blue")!=0 && strcmp(new_color, "Yellow")!=0);}
        else{// the player is a robot
          strcpy(new_color, (char*[]){"Red", "Green", "Blue", "Yellow"}[rand() % 4]);// the robot will choose a random color
           printf(" _________________________________________________________________________________\n");
          printf("                  the robot select the color %s\n",new_color);
           printf(" _________________________________________________________________________________\n");
        }


         // Remove the selected card from the player's hand
         remove_card(&((*current_player)->cards),&card_index);
        strcpy(color_card(*discard_stack), new_color);// updating the color of the first card in the discard satck

        if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);
        } else {
            *current_player = prev_player(*current_player);
        }
}
    }
    else {// in the case that the player doesnt player a special or action card
        // Regular card
        // here we check if the top card in the discard stack is particular
        if (strcmp(property_card(*discard_stack), "Reverse") == 0 ||
            strcmp(property_card(*discard_stack), "Pass") == 0 ||
            strcmp(property_card(*discard_stack), "+2") == 0 ||
            strcmp(property_card(*discard_stack), "+4") == 0 ||
            strcmp(property_card(*discard_stack), "Color Choice") == 0) {
             // Play the selected card
            printf("         %s played the card: ", name_player(*current_player));
            printf("\n");
            display_card(selected_card);
            printf("\n");
        // Remove the selected card from the player's hand
         remove_card(&((*current_player)->cards),&card_index);
           //Move the selected card to the top of the discard stack
            selected_card->next = *discard_stack;
            *discard_stack = selected_card;
        if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);
        } else {
            *current_player = prev_player(*current_player);
        }
}
        }
        else if (strcmp(color_card(selected_card), color_card(*discard_stack)) == 0 ||// to check  that the played card and the the card
            selected_card->number == number_card(*discard_stack) ) {    ////in the top of discared stack have the same color or number

            // Play the selected card
            printf("       %s played the card: ", name_player(*current_player));
            printf("\n");
            display_card(selected_card);
            printf("           %s played a regular card.\n",name_player(*current_player));
            // Remove the selected card from the player's hand
            remove_card(&((*current_player)->cards),&card_index);
            //Move the selected card to the top of the discard stack
            selected_card->next = *discard_stack;
            *discard_stack = selected_card;
            if((is_winner(*current_player)==0)){// Move to the next player
               if ((*direction) == 1) {
                    *current_player = next_player(*current_player);
            } else {
            *current_player = prev_player(*current_player);
            }
}

        }
        else{ // this part is used if the player choose a not playable card
        if(strcmp(id_player(*current_player),"robot")==0){// if the robot choose a unsuitable card we jump to debut to garentee that
                                                                 //the robot will play a suitable card
                        j=2;// we use this variable to don't repeat each time the statement'computer is thinking')
                        goto debut;// jump to debut
                          }
            // if the player is human
           printf("   invalide choice you can't play this card \n");
           printf("   your turn is finish!!!!!\n");// his turn finish
           sleep(2); //delay for 2 seconds
           if(strcmp(id_player(*current_player),"human")==0){// if the player is human and choose unsuitable card we move to the next player
               if((is_winner(*current_player)==0)){// Move to the next player
        if ((*direction) == 1) {
            *current_player = next_player(*current_player);
        } else {
            *current_player = prev_player(*current_player);
        }
        }
        }
        }
        }
        }






//--------------------------------------------------------------------------------------------------------------------------------------//

 void free_memory(player **players, card **deck, card **discard_stack) {
    // Free players
        free_players(players);
    // Free deck
    free_card(deck);
    // Free discard stack
   free_card(discard_stack);
 }
// Global flag to control background color change
int changeColor = 1;

// this function is just used  to change the background color
void *changeBackgroundColor(void *arg) {
    while (changeColor) {

        system("Color F0");// the white color
        sleep(2); // Sleep for 4 seconds
    }
    return NULL;
}
//also this part  is just to change the background color
        void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
        void Textecolor(int color) {
    printf("\033[3%dm", color);
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

  void welcome()
{
    int k;
    system("Cls");
    for (k = 0; k <3; k++)
    {
        if (k % 2 == 0)
            Textecolor(5);
        if (k % 2 == 1)
            Textecolor(3);
      system("color f1");
    int delay = 50;
    //remark:
    // this function is implemented by Melsi mounir and moumna charafeddine taken  from their tp (the last year)

    printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                  @@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@\n");Sleep(delay);
    printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@             @@@@@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@@@@@@@@@@@@@@@@@@@@@@           @@@@@@@       @@@@@@@@@@@           @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                 @@@@@         @@@@@              @@@@@@@            @@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                   @@@@        @@@@                 @@@@@@           @@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                    @@@       @@@@@                  @@@@@          @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@          @@@        @@       @@@@@@                   @@@          @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@          @@@                  @@@@@@@                  @@          @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@         @@@@                  @@@@@@@@@@@                          @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@        @@@@@                    @@@@@@@@@@@@@@@                    @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@@@@@@@@@@@@@@                      @@@@@@@@@@@@@@@@@                @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@@@@@@@@@@@@@@                         @@@@@@@@@@@@@@@@@@            @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@        @@@@@                               @@@@@@@@@@@@@@          @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@         @@@@                                     @@@@@@@@@         @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@          @@@                @@@@                    @@@@@@@        @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@          @@@         @@@    @@@@                     @@@@@@        @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                     @@@@     @@@@                     @@@@@        @@@@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                   @@@@@      @@@@@@                   @@@@@         @@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                 @@@@@@       @@@@@@                 @@@@@@          @@@@@\n");Sleep(delay);
    printf("\t\t     @@@@@@                @@@@@@@        @@@@@@@@@            @@@@@@          @@@@@@@\n");Sleep(delay);
    printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@\n");Sleep(delay);
    printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@\n\n");Sleep(delay);

    system("cls");
    }
    Textecolor(5);
    // this writing is taken from mabrouki hammani from here tp (the last year)
    printf("\t\t\t\t\t\t          R%cpublique Alg%crienne D%cmocratique et Populaire           \n", 130, 130, 130);
    printf("\t\t\t\t\t\tMinist%cre de l'Enseignement Sup%crieur et de la Recherche Scientifique\n\n\n", 130, 130);
    printf("\t\t\t\t\t\t     _______________________________________________________\n");
    printf("\t\t\t\t\t\t    |        _______     _______     _                      |\n");
    printf("\t\t\t\t\t\t    |       |  _____|   |  _____|   | |  %ccole nationale    |\n", 130);
    printf("\t\t\t\t\t\t    |       | |_____    | |_____    | |                     |\n");
    printf("\t\t\t\t\t\t    |       |  _____|   |_____  |   | |  sup%crieure         |\n", 130);
    printf("\t\t\t\t\t\t    |       | |_____     _____| |   | |                     |\n");
    printf("\t\t\t\t\t\t    |       |_______|   |_______|   |_|  d'informatique     |\n");
    printf("\t\t\t\t\t\t    |_______________________________________________________|\n\n\n");
    Textecolor(3);
    printf("\t\t\t\t\t\t\tCPI - 1%cre Ann%ce - Ann%ce Universitaire 2023/2024 \n\n\n", 138, 130, 130);
    printf("\t\t\t\t\t\t _______________________________________________________________________________________________\n");
    printf("\t\t\t\t\t\t|                                                                                               |\n");
    printf("\t\t\t\t\t\t| REALISED BY :                Rafa houssam                                                     |\n");
    printf("\t\t\t\t\t\t|                                                                                               |\n");
    printf("\t\t\t\t\t\t|                              bouderbala heytham                                               |\n");
    printf("\t\t\t\t\t\t|                                       &                                                       |\n");
    printf("\t\t\t\t\t\t|                               abada mahmoud                                                   |\n");
    printf("\t\t\t\t\t\t|           SECTION : D           GROUPE: 13                                                    |\n");
    printf("\t\t\t\t\t\t|                                                                                               |\n");
    printf("\t\t\t\t\t\t|           TP1 :                Linked Lists,stacks                                            |\n", 130, 130);
    printf("\t\t\t\t\t\t|                                                                                               |\n");
    printf("\t\t\t\t\t\t|                   < UNO GAME >                                                                |\n");
    printf("\t\t\t\t\t\t|                                                                                               |\n");
    printf("\t\t\t\t\t\t|                                                                                               |\n");
    printf("\t\t\t\t\t\t|                          Supervised By :Mrs. Genane Nabila                                    |\n");
    printf("\t\t\t\t\t\t|_______________________________________________________________________________________________|\n\n\n");
    Sleep(200);
    Sleep(200);
    Textecolor(5);
    printf("\n");
    printf("\t\t\t\t\t\t                             PLease                                         \n");
    printf("\t\t\t\t\t\t                    Press any key to continue..                             ");
    getch();
}
 void Welcome2()
{
    system("cls");
    for (int k = 0; k < 10; k++)
    {
        if (k % 2 == 0)
            Textecolor(5);
        if (k % 2 == 1)
            Textecolor(3);
            // this writing is taken from mabrouki hammani from here tp (the last year)
printf("\n\n\n\n\n\n\n");
printf("\t\t\t\t\t\t  .----------------.  .----------------.  .----------------. \n");
printf("\t\t\t\t\t\t | .--------------. || .--------------. || .--------------. |\n");
printf("\t\t\t\t\t\t | |  _________   | || |   ________   | || |     __       | |\n");
printf("\t\t\t\t\t\t | | |  _   _  |  | || |  |_   __  |  | || |    /  |      | |\n");
printf("\t\t\t\t\t\t | | |_| | | |_|  | || |    | |__) |  | || |    `| |      | |\n");
printf("\t\t\t\t\t\t | |     | |      | || |    |  ___/   | || |     | |      | |\n");
printf("\t\t\t\t\t\t | |    _| |_     | || |   _| |_      | || |    _| |_     | |\n");
printf("\t\t\t\t\t\t | |   |_____|    | || |  |_____|     | || |   |_____|    | |\n");
printf("\t\t\t\t\t\t | |              | || |              | || |              | |\n");
printf("\t\t\t\t\t\t | '--------------' || '--------------' || '--------------' |\n");
printf("\t\t\t\t\t\t '----------------'  '----------------'  '----------------' \n\n\n\n");
printf("\t\t\t\t\t\t .----------------.  .----------------.  .----------------.  \n");
printf("\t\t\t\t\t\t | .--------------. || .--------------. || .--------------. |\n");
printf("\t\t\t\t\t\t | |   _____      | || |   _____      | || |     ______   | |\n");
printf("\t\t\t\t\t\t | |  |_   _|     | || |  |_   _|     | || |   .' ___  |  | |\n");
printf("\t\t\t\t\t\t | |    | |       | || |    | |       | || |  / .'   |_|  | |\n");
printf("\t\t\t\t\t\t | |    | |   _   | || |    | |   _   | || |  | |         | |\n");
printf("\t\t\t\t\t\t | |   _| |__/ |  | || |   _| |__/ |  | || |  | `.___.'|  | |\n");
printf("\t\t\t\t\t\t | |  |________|  | || |  |________|  | || |   `._____.'  | |\n");
printf("\t\t\t\t\t\t | |              | || |              | || |              | |\n");
printf("\t\t\t\t\t\t | '--------------' || '--------------' || '--------------' |\n");
printf("\t\t\t\t\t\t '----------------'  '----------------'  '----------------'  \n");
Sleep(200);
system("cls");
}
    system("cls");
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//


void celebrate(player*winner) {// thi function  display the winner
    system("cls");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n                                    Congratulations! %s is  the winner!\n:     ",name_player(winner) );
    printf("                                                Let's celebrate!\n\n");
    sleep(4);
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    for (int k = 0; k < 10; k++)
    {
        if (k % 2 == 0)
            Textecolor(5);
        if (k % 2 == 1)
            Textecolor(3);
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\t\t\t\t\t\t  **          **      * ***************  **          **        \n");
    printf("\t\t\t\t\t\t  **          **      * *            **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *            **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *            **  **          **           \n");
    printf("\t\t\t\t\t\t  **************      * ***************  **************            \n");
    printf("\t\t\t\t\t\t  **************      *****************  **************           \n");
    printf("\t\t\t\t\t\t        **                                                          \n ");
    printf("\t\t\t\t\t\t        **                                                         \n");
    printf("\t\t\t\t\t\t        **                                                          \n");
    printf("\t\t\t\t\t\t        **                                                          \n ");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\t\t\t\t\t\t  **        **         **       **         **           **             \n");
    printf("\t\t\t\t\t\t  **        **         **       **         **  *        **           \n");
    printf("\t\t\t\t\t\t  **        **         **       **         **     *     **           \n");
    printf("\t\t\t\t\t\t  ***********************       **         **        *  **           \n");
    printf("\t\t\t\t\t\t  ***********************       **         **          ***           \n");
   Sleep(200);
system("cls");
}
sleep(2);// stop for 2 seconds
    system("cls");
}


//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//
                    // this writing is taken from mabrouki hammani from here tp (the last year)
   void GoodBye(){
            system("cls");
        for (int k = 0; k < 10; k++)
         {
               if (k % 2 == 0)
                  Textecolor(5);
               if (k % 2 == 1)
                  Textecolor(3);
printf("\n\n\n\n\n\n\n");
printf("\t\t\t\t .----------------.  .----------------.  .----------------.  .-----------------. .----------------. \n");
printf("\t\t\t\t| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n");
printf("\t\t\t\t| |  _________   | || |  ____  ____  | || |      __      | || | ____  _____  | || |  ___  ____   | |\n");
printf("\t\t\t\t| | |  _   _  |  | || | |_   ||   _| | || |     /  |     | || ||_   ||_   _| | || | |_  ||_  _|  | |\n");
printf("\t\t\t\t| | |_/ | | |_|  | || |   | |__| |   | || |    / /| |    | || |  | |   | |   | || |   | |_/ /    | |\n");
printf("\t\t\t\t| |     | |      | || |   |  __  |   | || |   / ____ |   | || |  | |---| |   | || |   |  __'.    | |\n");
printf("\t\t\t\t| |    _| |_     | || |  _| |  | |_  | || | _/ /    | |_ | || | _| |___| |_  | || |  _| |  | |_  | |\n");
printf("\t\t\t\t| |   |_____|    | || | |____||____| | || ||____|  |____|| || ||_____|(____| | || | |____||____| | |\n");
printf("\t\t\t\t| |              | || |              | || |              | || |              | || |              | |\n");
printf("\t\t\t\t| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n");
printf("\t\t\t\t '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n\n\n");
printf("\t\t\t\t           .----------------.  .----------------.  .----------------.  .----------------. \n");
printf("\t\t\t\t          | .--------------. || .--------------. || .--------------. || .--------------. |\n");
printf("\t\t\t\t          | |  ____  ____  | || |     ____     | || | _____  _____ | || |              | |\n");
printf("\t\t\t\t          | | |_  _||_  _| | || |   .'    `.   | || ||_   _||_   _|| || |      _       | |\n");
printf("\t\t\t\t          | |   ( |  / /   | || |  /  .--.  |  | || |  | |    | |  | || |     | |      | |\n");
printf("\t\t\t\t          | |    ( |/ /    | || |  | |    | |  | || |  | '    ' |  | || |     | |      | |\n");
printf("\t\t\t\t          | |    _|  |_    | || |  |  `--'  /  | || |   | `--' /   | || |     | |      | |\n");
printf("\t\t\t\t          | |   |______|   | || |   `.____.'   | || |    `.__.'    | || |     |_|      | |\n");
printf("\t\t\t\t          | |              | || |              | || |              | || |     (_)      | |\n");
printf("\t\t\t\t          | '--------------' || '--------------' || '--------------' || '--------------' | \n");
printf("\t\t\t\t           '----------------'  '----------------'  '----------------'  '----------------' \n");
Sleep(200);
system("cls");
}
    system("cls");
}
void reshuffleWithProgress() {// this function is just to show the progress of shuffling process
    int total_progress = 100; // Total progress percentage
    int progress_increment = 20; // Increment for each update
    int progress = 0; // Current progress percentage

    // Print initial progress message
    printf("Please wait... Shuffling... %d%% Complete.", progress);
    fflush(stdout); // Flush output buffer to ensure the message is displayed

    while (progress < total_progress) {
        // Simulate some processing time
        usleep(1000000); // Sleep for 500 milliseconds (0.5 seconds)

        // Update progress
        progress += progress_increment;

        // Ensure progress doesn't exceed 100%
        if (progress > total_progress) {
            progress = total_progress;
        }

        // Print updated progress message
        printf("\rPlease wait... Shuffling... %d%% Complete.", progress);
        fflush(stdout); // Flush output buffer to ensure the message is displayed
    }

    // Print completion message
    printf("\n");
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

 void play_uno_game(card * deck){// this is the main function which manage the game until the end
     system("cls");//to open a new empty window for the user
     int nbr;
     do{ printf("  please enter the number of players : \n");
     scanf("%d",&nbr);
     if(nbr<=1 || nbr >4)
        printf(" the number of player must be greater than 1 and less than 4\n");
     } while (nbr<=1 || nbr >4 ); // the number of player must be less than 4
    card *discard_stack = NULL;
    reshuffleWithProgress();// to display the progress of shuffling
    // Initialize players
    player *players = NULL;
    initialize_players(&players, nbr, &deck,&discard_stack);// make all players ready to play uno game
    display_players(players);// displaying the players
   // Start the game by drawing the first card
    start_game(&deck, &discard_stack);

    // Set initial direction of play
    int direction=1 ; // 1 for clockwise, -1 for counterclockwise

    // Set the current player to the first player
    player *current_player = players;

    // Main game loop
    while (is_winner(players)==0) {// we stop the game until one player win


        // Display current player's turn
        printf("\n\n----------------------------------------%s's turn:\n------------------------------",name_player(current_player));

        // Play card for the current player
        play_card(&current_player, &discard_stack, &deck, &direction);
        if(is_winner(players)==1)// there exist a winner then the game stopped
            break;


        }

    // Declare the winner
    printf("\n------------------------------------------------------------Game over!-------------------------------------------------\n");

     celebrate(current_player);
   printf("---------------------------------------------------------------------------------------------------------------------------");

    // Free memory
    free_memory(&players,&deck,&discard_stack);// freeing memory


}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

void displayRules() {// this function is used toopen external file which contain uno rules
    // Open and display rules from rules.txt file
    FILE *file = fopen("uno_rules.txt", "r");
    if (file != NULL) {
        int c;
        while ((c = fgetc(file)) != EOF) {
            putchar(c);
            fflush(stdout); // Flush output buffer

            // Print a high typing sound
            putchar('\a');
            fflush(stdout); // Flush output buffer
            usleep(20000); // Shorter delay for higher pitch sound
        }
        fclose(file);
    }
    else
    {
        printf("   please put the file named uno_rules.txt with the program\n   ");
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

void uno_logo(){
 int i,k=0,ch,pos,count=1;
    char c;
    int j=1;
    system("cls");
        if (k % 2 == 0)
            Textecolor(5);
        if (k % 2 == 1)
            Textecolor(3);
    for(i=1;i<=5;i++){
    Sleep(200);
    system("cls");
    printf("\n");
     printf("\n");
      printf("\n");
       printf("\n");
        printf("\n");
         printf("\n");
         printf("\n");
    printf("\t\t\t\t\t\t  **          **        *            **   ************           \n");
    printf("\t\t\t\t\t\t  **          **      * *  *         **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *     *      **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *       *    **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *          * **  **          **           \n");
    printf("\t\t\t\t\t\t  **************      * *            **  **          **           \n");
    printf("\t\t\t\t\t\t   ************       * *             *   ************            \n");
    printf("\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t L O A D I N G ....... %d %% \n",(j++)*20);
    Beep(800,500);



    Sleep(1000);
    system("cls");
}
    Beep(500,400);
    Beep(750,500);
    Beep(1000,500);


    printf("\t\t\t\t\t\t  **          **      * *            **  **************          \n");
    printf("\t\t\t\t\t\t  **          **      * *  *         **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *     *      **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *       *    **  **          **           \n");
    printf("\t\t\t\t\t\t  **          **      * *          * **  **          **           \n");
    printf("\t\t\t\t\t\t  **************      * *            **  **          **           \n");
    printf("\t\t\t\t\t\t  **************      * *            **  **************           \n");

    printf("\n\n\t\t\t\t\t\t\t   *******  T H E     B A T T L E     O F     C A R D S  *******\n");
    Sleep(500);
    Sleep(200);
system("cls");
}
void openPDF() {// this function is used to open a pdf document from the C program

    const char *pdfFileName = "report_TP.pdf";

    // Command to open PDF file with the default PDF viewer
    #ifdef _WIN32
        const char *command = "start";
    #else
        const char *command = "xdg-open";
    #endif

    // Construct the full command with the PDF file name
    char fullCommand[1024];
    #ifdef _WIN32
        snprintf(fullCommand, sizeof(fullCommand), "%s %s", command, pdfFileName);
    #else
        snprintf(fullCommand, sizeof(fullCommand), "%s %s", command, pdfFileName);
    #endif

    // Execute the command
    system(fullCommand);
}
//--------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------//

int main() {

    printf("please maximize the screen of the excution of the program to avoid displaying problems\n\n\n");
    printf(" press any key to continue \n");
    getch();
    card * deck= generate_deck();// initialization of the deck
    shuffle_deck(&deck);
    // this 2 lines is just used to to different action in a same time (change background color and runing the game

   pthread_t bgThread;
    pthread_create(&bgThread, NULL, changeBackgroundColor, NULL);

    // Display welcome messages and logo with colors
    welcome();
    Welcome2();
    uno_logo();


    // Stop background color change after 3 functions
    changeColor = 0;
    pthread_join(bgThread, NULL); // Wait for background color change thread to finish


     srand(time(NULL));// to garentee the random choosing of cards each new game

    int choice = 0;
    int num_choices = 5;
    char *choices[] = {
        "                                                        1) Play Uno Game",
        "                                                        2) Display Rules",
        "                                                        3) display the deck",
        "                                                        4) display the report of the TP",
        "                                                        5) End"
    };

    while (1) {
        system("cls"); // Clear the screen (for Windows)

        // Print menu with options and highlight the selected one
        printMenu(choice, num_choices, choices);

        // Capture arrow key presses for navigation
        int key = getch();
        if (key == 224) { // Arrow key detected
            key = getch(); // Read the next character to determine the arrow key
            if (key == 72 && choice > 0) { // Up arrow
                choice--;
            } else if (key == 80 && choice < num_choices - 1) { // Down arrow
                choice++;
            }
        } else if (key == '\r') { // Enter key
            // Perform action based on selected option
            if (choice == 0) {
                printf("Starting Uno game...\n");
                play_uno_game(deck);
                GoodBye();

                // Play Uno Game
            } else if (choice == 1) {

                printf("Displaying rules...\n");

                // Display Rules
                displayRules();
            } else if (choice == 2) {

                printf("Displaying the deck...\n");

                // Display deck
                display_deck(deck);
            }else if (choice == 3) {

                printf("Displaying the report of the TP...\n");

                // Display report
               openPDF();
            }else if (choice == 4) {

                printf("Exiting...\n");
                GoodBye();
                break; // End
            }
            printf("Press Enter to continue...");
            getch(); // Wait for Enter key
        }
    }

    return 0;
}
//                                                          ********************************************************
//                                                         **********************************************************
//---------------------------------------------------------*       end of the program:thank you for your time!!!!   *---------------------------------------------------
//                                                         **********************************************************
//                                                          ********************************************************
