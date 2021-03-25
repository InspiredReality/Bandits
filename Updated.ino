enum signalStates {INERT, GO, RESOLVE};  //00, 01, 10 in [A] [B]
byte signalState = INERT;

enum bidScoreComms {NONE, ONE, TWO, THREE}; //00, 01, 10 in [A] [B]
byte bidScoreComm = ONE;
byte bidScore = 1;
byte blankbidComms = 00;

//checkWinner
bool winner = true;
bool isDiamond = false;
bool isTreasure = false;
bool isConduit = false;
bool onDiamond = false;

//byte prizeSignal = 0;
byte winningFace = 6;
byte diamondFace = 6;
byte banditFace = 6;
byte remainingTreasure = 0;
byte pulseFace = 0;
bool reset = false;

bool isRevealed = false;
Timer revealTimer;
#define REVEAL_INTERVAL 1000
#define REVEAL_FADE 500
Timer waitThenReveal;
#define WAIT_THEN_REVEAL 3000
int waitingFace = 0;
Timer waitingFaceTimer;

Timer sparkleTimer;
#define SPARKLE_DURATION 100
Color displayColor[] = {OFF,OFF,OFF,OFF,OFF,OFF};
Timer facetFaceRotationTimer;
int baseFacetFace=0;

Color teamColors[6] = {RED, ORANGE, YELLOW, GREEN, CYAN, MAGENTA};
byte teamColor = 0;

void setup() {
}


void loop() {
  switch (signalState) {
    case INERT:
      inertLoop();
      break;
    case GO:
      goLoop();
      break;
    case RESOLVE:
      resolveLoop();
      break;
  }

  //do display
  if (isDiamond) {
    diamondVisuals();
  } 
  else if (isTreasure || isConduit){
    treasureVisuals();
  }
  else {
    banditVisuals();
  }


 //do comms
   switch (bidScore) {
    case 0:
      bidScoreComm = NONE;
      break;
    case 1:
      bidScoreComm = ONE;
      break;
    case 2:
      bidScoreComm = TWO;
      break;
    case 3:
      bidScoreComm = THREE;
      break;
  }
      
  if (isDiamond) {
      FOREACH_FACE(f) {
        byte sendData;
        if (winningFace == f) {
          sendData = (signalState << 4) + (bidScoreComm << 2) + (winner << 1) + (isDiamond);
        }
        else {
          sendData = (signalState << 4) + (bidScoreComm << 2) + (!winner << 1) + (isDiamond);
        }
        setValueSentOnFace(sendData, f);
      }
    } 
      
  //dump button presses
  buttonSingleClicked();
  buttonDoubleClicked();
  buttonMultiClicked();
}

void inertLoop() {
  //BANDIT
  if(!isDiamond && !isTreasure && !isConduit){
    
    if (buttonMultiClicked()) {
      isTreasure = false;
      isDiamond = true;
      is conduit = false;
      bidScore = 0;
      bidScoreComm = NONE;
    }
  
    if (buttonSingleClicked()) {
      //if I'm currently visible, increment count. Otherwise, just become revealed
      if (isRevealed) {
        bidScore += 1;
        if (bidScore > 3) {
          bidScore = 1;
        }
      }
      revealTimer.set(REVEAL_INTERVAL + REVEAL_FADE);
      isRevealed = true;
    }
    if (revealTimer.isExpired()) {
      isRevealed = false;
    }
  
    if (buttonDoubleClicked()) {
      teamColor = (teamColor + 1) % 6;
    }
    
    pulseFace = 6;
    //Bandit listen for GO to reveal bid and determine if we won
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
        byte neighborData = getLastValueReceivedOnFace(f);
        if(getSignalState(neighborData) == GO){
          signalState = GO;
          
        }

switch (getBidScore(neighborData)) {
    case NONE:
      bidScoreComm = NONE;
      break;
    case ONE:
      bidScoreComm = ONE;
      break;
    case TWO:
      bidScoreComm = TWO;
      break;
    case THREE:
      bidScoreComm = THREE;
      break;

        //if stealing from a 1 or 2, then you just block and stay attached
    //    if(getBidScore(neighborData) == ONE || getBidScore(neighborData) == TWO){
     //     remainingTreasure = 0;
      //    //send locked score animation up the chain to the Diamond
     //   }
      //  else {
       //   remainingTreasure = 4 - bidScore;
      //  }
        if(checkWinner(neighborData) == true){
          isTreasure = true;
          diamondFace = f;   
          banditFace = (diamondFace + 3) % 6;
        //  bidScore = 0;  
       //   bidScoreComm = NONE;           
        } 
        if(getDiamond(neighborData) == true){
          onDiamond = true;
          pulseFace = f;
        }
      }
    }

  if (isTreasure){
    if (onDiamond){
      remainingTreasure = 6 - bidScore;
    }
  else if (bidScoreComm = TWO || bidScoreComm = ONE{
    remainingTreasure = 4 - bidScore;
  }
  }
  if(signalState == GO) {
    waitThenReveal.set(WAIT_THEN_REVEAL);
    revealTimer.set(6000);
  }
  
  }
  
//        //if neighbor score = 0 then not another Bandit so pulse
//        if(getBidScore(neighborData) == 0){
//          pulseFace = f;
//        }
  
        //GO so check if winner and calculate score
//        if (getSignalState(neighborData) == GO && getDiamond(neighborData) == true && checkWinner(neighborData) == true){ //GO Time!
//          signalState = GO;
//          //queue bid reveal
//          waitThenReveal.set(WAIT_THEN_REVEAL);
//          revealTimer.set(6000);
          
//          //We win this round
//          if (getDiamond(neighborData) == true && checkWinner(neighborData) == true){ //diamond winner
//            //diamond neighbor
//            remainingTreasure = 6 - bidScore;
//          }
//          else if (getDiamond(neighborData) == false && checkWinner(neighborData) == true){ //treasure winner
//            //treasure neighbor so check treasure's remainingTreasure
//            if(getBidScore(neighborData) > 2){
//              remainingTreasure = 4 - bidScore;
//            }
//            //if stealing from a 1 or 2, then you just block and stay attached
//            else {
//              remainingTreasure = 0;
//              //send locked score animation up the chain to the Diamond
//            }
//          }
//          diamondFace = f;   
//          banditFace = (diamondFace + 3) % 6;
//          isTreasure = true;
//          bidScore = 0;  
//          bidScoreComm = NONE;
//        }
////        else if (getSignalState(neighborData) == GO && getDiamond(neighborData) == true && checkWinner(neighborData) == true){ //GO Time!
//      }
//    }
//  }

  
  //DIAMOND
  else if(isDiamond){
  
    if (buttonMultiClicked()) {
      if(buttonClickCount() == 3)
      {
        isTreasure = false;
        isDiamond = false;
        bidScore = 1;
      }
      if(buttonClickCount() == 4)
      {
        reset = true;
      }
    }

    byte bidCount[3] = {0, 0, 0};//this tells me how many bids of 1/2/3 we receive
    byte bidLocation[3] = {6, 6, 6};//this tell me where the bid is located, defaulting to 6 because that's nowhere

    //run through the faces and fill out these arrays
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
        byte neighborData = getLastValueReceivedOnFace(f);
//        if (getBidScore(neighborData) == 0) {
//          //add to pulse face array
//        }
        byte thisBid = getBidScore(neighborData);
        bidCount[thisBid - 1] += 1;//increment the count for this type of bid
        bidLocation[thisBid - 1] = f;//set this as the location for that bid. Overwriting is fine because duplicates can't score anyway
//        }
      }
    }

    //now determine the winner and where it is located
    winningFace = 6;//default to 6 because that's no one
    //if only one 3 bid then it wins
    if (bidCount[2] == 1) {
      winningFace = bidLocation[2];
    } else if (bidCount[1] == 1) {
      winningFace = bidLocation[1];
    } else if (bidCount[0] == 1) {
      winningFace = bidLocation[0];
    }

    if (buttonDoubleClicked()) {//ok, so this is where we do the reveal
      signalState = GO;
      //play checkWinnerTimer
      waitThenReveal.set(WAIT_THEN_REVEAL);
    }
  }

  //TREASURE
  else if(!isDiamond && isTreasure && !isConduit){
    if (!isValueReceivedOnFaceExpired(banditFace)) {//neighbor on banditFace trying to steal, so pass along bid
      byte neighborData = getLastValueReceivedOnFace(banditFace);
      bidScore = getBidScore(neighborData);
    }
  }
  
  
//  //CONDUIT
//
//  else if(isConduit){
//    
//  }

}

void goLoop() {
    signalState = RESOLVE;//I default to this at the start of the loop. Only if I see a problem does this not happen  

//  look for neighbors who have not heard the GO news
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getSignalState(getLastValueReceivedOnFace(f)) == INERT) {//This neighbor doesn't know it's GO time. Stay in GO
        signalState = GO;
      }
    }
  }
}

void resolveLoop() {
    signalState = INERT;//I default to this at the start of the loop. Only if I see a problem does this not happen

  //look for neighbors who have not moved to RESOLVE
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getSignalState(getLastValueReceivedOnFace(f)) == GO) {//This neighbor isn't in RESOLVE. Stay in RESOLVE
        signalState = RESOLVE; 
      }
    }
  }
}


void diamondVisuals() {
  //default OFF to stop color lingering
  setColor(OFF);
    
  switch (signalState) {
    case INERT:
      if( sparkleTimer.isExpired() ){
        sparkleTimer.set(10);
        fullSparkle();
        FOREACH_FACE(f) {
          setColorOnFace(displayColor[f],f);
        }
      }
      if (!waitThenReveal.isExpired()){
        //running timer and not my first time as Treasure or TeamTreasure so circle spin animation
        setColorOnFace(dim(WHITE,150),waitingFace % 6);
        setColorOnFace(WHITE,(waitingFace+1) % 6);
        setColorOnFace(dim(WHITE,150),(waitingFace+2) % 6);
        setColorOnFace(OFF,(waitingFace+3) % 6);
        setColorOnFace(OFF,(waitingFace+4) % 6);
        setColorOnFace(OFF,(waitingFace+5) % 6);
        if( waitingFaceTimer.isExpired() ){
          waitingFaceTimer.set(500);
          waitingFace ++;
        }
      }

      
      break;
    case GO:
      setColor(MAGENTA);
      break;
    case RESOLVE:
      setColor(WHITE);
      break;
  }
}

void fullSparkle(){

  int whiteShine = random(5);
  int whiteShine2 = random(5)-3;
//  int whiteShine3 = random(2);
//  int blueShine = random(2);
//  int purpShine = random(5);
  Color blue2 = makeColorRGB(84,204,255);
  Color purp = makeColorRGB(134,125,255);
  
  displayColor[baseFacetFace] = blue2;
  displayColor[(baseFacetFace+1)%6] = purp;
  displayColor[(baseFacetFace+2)%6] = blue2;
  displayColor[(baseFacetFace+3)%6] = purp;
  displayColor[(baseFacetFace+4)%6] = blue2;
  displayColor[(baseFacetFace+5)%6] = purp;
//  displayColor[purpShine] = purp;    
  displayColor[whiteShine] = WHITE;
//  displayColor[blueShine] = blue;
//  displayColor[(blueShine+2)%6] = blue;
  displayColor[whiteShine2] = WHITE;
//  displayColor[whiteShine3] = WHITE;
  if( facetFaceRotationTimer.isExpired() ){
    facetFaceRotationTimer.set(100);
    baseFacetFace ++;
//    baseFacetFace = baseFacetFace % 6;
  }
}

void treasureVisuals(){
  
}

void banditVisuals(){
  setColor(OFF);
  if (waitThenReveal.isExpired()) {
    //not waiting to reveal
    if (isRevealed) {
      FOREACH_FACE(f) {
        if (bidScore >= 1) {
          setColorOnFace(teamColors[teamColor], 0);
          setColorOnFace(teamColors[teamColor], 1);        
        } 
        if (bidScore >= 2) {
          setColorOnFace(teamColors[teamColor], 2);
          setColorOnFace(teamColors[teamColor], 3);        
        } 
        if (bidScore >= 3) {
          setColorOnFace(teamColors[teamColor], 4);
          setColorOnFace(teamColors[teamColor], 5);        
        } 
        else if (revealTimer.getRemaining() < REVEAL_FADE && !revealTimer.isExpired()) {
          byte fadeLevel = 150 - map(revealTimer.getRemaining(), 0, REVEAL_FADE, 0, 150);
          setColorOnFace(dim(teamColors[teamColor], random(fadeLevel)), f);
        }
      }
    }
    else {
      FOREACH_FACE(f) {
        setColorOnFace(dim(teamColors[teamColor], random(150)), f);
      }
    }
  }
  else {
    //spin team color while Waiting to Reveal
    setColorOnFace(dim(teamColors[teamColor],150),waitingFace % 6);
    setColorOnFace(teamColors[teamColor],(waitingFace+1) % 6);
    setColorOnFace(dim(teamColors[teamColor],150),(waitingFace+2) % 6);
    setColorOnFace(OFF,(waitingFace+3) % 6);
    setColorOnFace(OFF,(waitingFace+4) % 6);
    setColorOnFace(OFF,(waitingFace+5) % 6);
    if( waitingFaceTimer.isExpired() ){
      waitingFaceTimer.set(500);
      waitingFace ++;
    }
  }
  if(pulseFace < 6) {
    setColorOnFace(WHITE,pulseFace);
  }
}

byte getSignalState(byte data) {
    return ((data >> 4) & 3);//returns bits A & B
}

byte getBidScore(byte data) {
    return ((data >> 2) & 3);//returns bits C & D
}

byte checkWinner(byte data) {
    return (data >> 1);//returns bit E
}

byte getDiamond(byte data) {
    return (data);//returns bit F
}
