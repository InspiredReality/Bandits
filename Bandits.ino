enum signalStates {INERT, GO, RESOLVE, BANDIT};  //00, 01, 10 in [A] [B]
byte signalState = BANDIT;

enum bidScoreComms {NONE, ONE, TWO, THREE}; //00, 01, 10 in [A] [B]
byte bidScoreComm = ONE;
byte bidScore = 1;

enum winnerStates {WINNER, LOSER}; 
byte winnerState = LOSER;


//bool isDiamond = false;

//checkWinner
//bool firstTimeTreasure = true;
//bool canWin = false;
bool winner = true;
bool onDiamond = false;
bool endRound = false;
byte toBandit;
byte toDiamond;
byte fromDiamondTreasure;
byte fromBandit;
//bool canSteal = true;
bool flagYellow = false;

//Identity
//enum role {DIAMOND, TREASURE, CONDUIT, BANDIT};
//byte role = BANDIT;
bool isDiamond = false;
bool isTreasure = false;
bool isConduit = false;


//byte prizeSignal = 0;
byte winningFace = 6;
byte facingDiamondFace = 6;
byte banditFace = 6;
byte remainingTreasure = 0;
//byte pulseFace = 6;
bool reset = false;

bool isRevealed = false;
Timer revealTimer;
#define REVEAL_INTERVAL 1000
#define REVEAL_FADE 500
Timer waitThenReveal;
Timer treasureWaitThenReveal;
#define WAIT_THEN_REVEAL 1500
int waitingFace = 0;
Timer waitingFaceTimer;
#define WAITING_FACE_CHANGE 250
Timer diamondSendWinnerTimer;
#define SEND_WINNER 500
Timer showBidTimer;
#define SHOW_BID 6000
Timer switchColorTimer;
#define PULSE_LENGTH 2000
bool useWhite = true;

Timer sparkleTimer;
#define SPARKLE_DURATION 100
Color displayColor[] = {OFF,OFF,OFF,OFF,OFF,OFF};
Timer facetFaceRotationTimer;
int baseFacetFace=0;

Color teamColors[6] = {RED, ORANGE, YELLOW, GREEN, CYAN, MAGENTA};
byte teamColor = 0;

void setup() {
  waitThenReveal.set(10);
  switchColorTimer.set(PULSE_LENGTH);  
}


void loop() {
  if (switchColorTimer.isExpired()){
    useWhite = !useWhite;
    switchColorTimer.set(PULSE_LENGTH);
  }
  
  switch (signalState) {
    case BANDIT:
      banditLoop();
      break;
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
//  if (signalState == GO){
//      setColor(MAGENTA);
//  }
//  if (signalState == RESOLVE){
//      setColor(BLUE);
//  }
//  if (signalState == BANDIT){
//    setColorOnFace(BLUE,3);
//  }
//  if (signalState == INERT){
//    setColorOnFace(MAGENTA,3);
//  }
//  if(endRound){
//    setColorOnFace(BLUE,3);
//  }
  if (flagYellow){
    setColor(YELLOW);
  }
  
//  if (signalState == INERT){
//      setColorOnFace(YELLOW, 5);
//  }

 //do comms    
  if (isDiamond) {
      FOREACH_FACE(f) {
        byte sendData;
        if (winningFace == f) {
          winnerState = WINNER;
          sendData = (signalState << 4) + (bidScore << 2) + (winnerState << 1) + (isDiamond);
//          flagYellow = true;
        }
        else {
          winnerState = LOSER;
          sendData = (signalState << 4) + (bidScore << 2) + (winnerState << 1) + (isDiamond);
        }
        setValueSentOnFace(sendData, f);
      }
    } 
    else if (isTreasure || isConduit) {
      setValueSentOnAllFaces(0);
      byte sendData;

          sendData = (signalState << 4) + (toDiamond << 2) + (winnerState << 1) + (isDiamond);  
          setValueSentOnFace(sendData, facingDiamondFace);
          sendData = (signalState << 4) + (toBandit << 2) + (winnerState << 1) + (isDiamond);
          setValueSentOnFace(sendData, banditFace);
    }
    else {
//      signalState = IGNORE;
        byte sendData = (signalState << 4) + (bidScore << 2) + (0 << 1) + (isDiamond);
        setValueSentOnAllFaces(sendData);
//        if(facingDiamondFace < 6){
//          sendData = (signalState << 4) + (bidScore << 2) + (0 << 1) + (isDiamond);
//          setValueSentOnFace(sendData, facingDiamondFace);
//        }
    }
  
  //dump button presses
  buttonSingleClicked();
  buttonDoubleClicked();
  buttonMultiClicked();
}

void inertLoop(){
//  if(!isDiamond && !isTreasure && !isConduit){
//    banditLoop();
//    }
  if(isDiamond){
    diamondLoop();
    }
  else if(isTreasure && !isConduit){
    treasureLoop();
    }
  else if(isConduit){
    conduitLoop();
    }
}

void banditLoop(){
        if (buttonMultiClicked()) {
        isTreasure = false;
        isDiamond = true;
        isConduit = false;
        bidScore = 0;
        signalState = INERT;
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
  
  
  
//      pulseFace = 6;
      facingDiamondFace = 6;
      onDiamond = false;
      //Bandit listen for GO to reveal bid and determine if we won
      //check if we can win  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
    byte neighborData = getLastValueReceivedOnFace(f);
      if (getDiamond(neighborData) == true){

        facingDiamondFace = f;
        onDiamond = true; 
        }
      else if (!onDiamond && getSignalState(neighborData) != BANDIT ){ 

        facingDiamondFace = f;
      }
    }
  }
   
  if (facingDiamondFace < 6){
    byte diamondNeighborData = getLastValueReceivedOnFace(facingDiamondFace);
    if(getSignalState(diamondNeighborData) == GO ){
      waitThenReveal.set(WAIT_THEN_REVEAL);
      showBidTimer.set(6000);
      
      if(checkWinner(diamondNeighborData) == WINNER ){
        banditFace = (facingDiamondFace + 3) % 6;
        isTreasure = true;

        //calculate remaining Treasure
        if (onDiamond){
          remainingTreasure = 6 - bidScore;  
        }
        else if (getBidScore(diamondNeighborData) == 1) {
          //1 means Treasure has more than 2 remaining
          remainingTreasure = 4 - bidScore;
        }
        else if (getBidScore(diamondNeighborData) == 0) {
          //0 means Treasure has only 1 or 2 remaining so can't be stolen
          remainingTreasure = 0;
        }
      }
      signalState = GO;
    }
  }

     
}

void diamondLoop() {
  //DIAMOND 
    if (buttonMultiClicked()) {
      if(buttonClickCount() == 3)
      {
        becomeBandit();
      }
      if(buttonClickCount() == 4)
      {
        reset = true;
      }
    }

    byte bidCount[3] = {0, 0, 0};//this tells me how many bids of 1/2/3 we receive
    byte bidLocation[3] = {6, 6, 6};//this tell me where the bid is located, defaulting to 6 because that's nowhere
   
    //only if not waiting to reveal, then calculate winningFace and allow 2x click to enter GO 
    if (waitThenReveal.isExpired()){
      winningFace = 6;//default to 6 because that's no one
      //run through the faces and fill out these arrays
      FOREACH_FACE(f) {
        if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
          byte neighborData = getLastValueReceivedOnFace(f);
          byte thisBid = getBidScore(neighborData);
          bidCount[thisBid - 1] += 1;//increment the count for this type of bid
          bidLocation[thisBid - 1] = f;//set this as the location for that bid. Overwriting is fine because duplicates can't score anyway
  //        }
        }
      }
    
      //now determine the winner and where it is located
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
        diamondSendWinnerTimer.set(SEND_WINNER);
      }
      
    }
}

void treasureLoop(){
  //TREASURE
    if (buttonMultiClicked()) {
      becomeBandit();
    }
    
    byte diamondNeighborData = getLastValueReceivedOnFace(facingDiamondFace);
    if (!isValueReceivedOnFaceExpired(banditFace)) { //neighbor has bandit face trying to steal, so pass along bid
      byte banditNeighborData = getLastValueReceivedOnFace(banditFace);
      toDiamond = getBidScore(banditNeighborData);
      toBandit = 0;
      winnerState = LOSER;
      
      if (getSignalState(diamondNeighborData) == GO) {
        signalState = GO;
        diamondSendWinnerTimer.set(SEND_WINNER);
        if (checkWinner(diamondNeighborData) == WINNER) {
          isConduit = true;
          winnerState = WINNER;
          treasureWaitThenReveal.set(WAIT_THEN_REVEAL);
          if(remainingTreasure < 3) {
            toBandit = 0;
          }
          else{
            toBandit = 1;
            remainingTreasure = remainingTreasure - (4 - toDiamond);
          }
        }
      }
    }
    
  }

void conduitLoop(){
//  //CONDUIT
    if (buttonMultiClicked()) {
      becomeBandit();
    }
    
    if (!isValueReceivedOnFaceExpired(banditFace)) { //neighbor has bandit face trying to steal, so pass along bid
      byte banditNeighborData = getLastValueReceivedOnFace(banditFace);
      toDiamond = getBidScore(banditNeighborData);
      byte diamondNeighborData = getLastValueReceivedOnFace(facingDiamondFace);
      toBandit = 0;
      winnerState = LOSER;
      
      if (getSignalState(diamondNeighborData) == GO) {
        signalState = GO;
        diamondSendWinnerTimer.set(SEND_WINNER);
        if (checkWinner(diamondNeighborData) == WINNER) {
          winnerState = WINNER;
        }
      }
    }
  }

void goLoop() {
  //GO is the SEND_WINNER
   
   if(!isDiamond && !isTreasure && !isConduit){
     signalState = BANDIT;
   }
   else if (isDiamond && !diamondSendWinnerTimer.isExpired()){
    signalState = GO;
   }
   else{
    signalState = RESOLVE;
   }
}

void resolveLoop() {
  signalState = INERT;
}


void diamondVisuals() {
  setColor(OFF);

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
      waitingFaceTimer.set(WAITING_FACE_CHANGE);
      waitingFace ++;
    }
  }

  if (winningFace != 6) {
    setColorOnFace(RED, winningFace);
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
  if (!treasureWaitThenReveal.isExpired()){
  }
  else
  {
    setColor(OFF);    
  //  if (firstTimeTreasure){
//    firstTimeTreasure = false;
//    waitThenReveal.set(WAIT_THEN_REVEAL);
//  }
    if (!waitThenReveal.isExpired()){
      //spin team color while Waiting to Reveal
      setColorOnFace(dim(teamColors[teamColor],150),waitingFace % 6);
      setColorOnFace(teamColors[teamColor],(waitingFace+1) % 6);
      setColorOnFace(dim(teamColors[teamColor],150),(waitingFace+2) % 6);
      setColorOnFace(OFF,(waitingFace+3) % 6);
      setColorOnFace(OFF,(waitingFace+4) % 6);
      setColorOnFace(OFF,(waitingFace+5) % 6);
      if(waitingFaceTimer.isExpired() ){
        waitingFaceTimer.set(WAITING_FACE_CHANGE);
        waitingFace ++;
      }
    }
    else {     
      FOREACH_FACE(f) {
        if(useWhite){
          setColorOnFace(dim(WHITE, random(150)), f);   
        }
        else{
          setColorOnFace(dim(makeColorRGB(84,204,255), random(150)), f);          
        }     
      }
      FOREACH_FACE(f) {
        if (f < remainingTreasure) {
          setColorOnFace(teamColors[teamColor], f);
        }
      }
    }
  }
}

void banditVisuals(){
  setColor(OFF);
  if (waitThenReveal.isExpired()) {
    //not waiting to reveal
    if(!showBidTimer.isExpired()){
      FOREACH_FACE(f) {
        if (bidScore > f ) {
          setColorOnFace(teamColors[teamColor], f);
        }
      } 
    }
    else if (isRevealed) {
      FOREACH_FACE(f) {
        if (bidScore > f ) {
          setColorOnFace(teamColors[teamColor], f);
        } 
        else if (revealTimer.getRemaining() < REVEAL_FADE && !revealTimer.isExpired()) {
          byte fadeLevel = 300 - map(revealTimer.getRemaining(), 0, REVEAL_FADE, 0, 150);
          setColorOnFace(dim(teamColors[teamColor], random(fadeLevel)), f);
        }
      }
    }
    else {
      FOREACH_FACE(f) {
        setColorOnFace(dim(teamColors[teamColor], random(150)), f);
      }
      if (facingDiamondFace < 6){
//        setColorOnFace(GREEN, facingDiamondFace);
        //get progress from 0 - MAX
        int pulseProgress = millis() % PULSE_LENGTH;
      
        //transform that progress to a byte (0-255)
        byte pulseMapped = map(pulseProgress, 0, PULSE_LENGTH, 0, 255);
      
        //transform that byte with sin
        byte dimness = sin8_C(pulseMapped);
  
        if(useWhite){
          setColorOnFace(dim(WHITE, dimness), facingDiamondFace);   
        }
        else{
          setColorOnFace(dim(teamColors[teamColor], dimness), facingDiamondFace);          
        }
        
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
      waitingFaceTimer.set(WAITING_FACE_CHANGE);
      waitingFace ++;
    }
  }
}

void becomeBandit(){
  isTreasure = false;
  isDiamond = false;
  isConduit = false;
  bidScore = 1;
//  firstTimeTreasure = true;
  signalState = BANDIT;
  facingDiamondFace = 6;
}

        
byte getSignalState(byte data) {
    return ((data >> 4) & 3);//returns bits A & B
}

byte getBidScore(byte data) {
    return ((data >> 2) & 3);//returns bits C & D
}

byte checkWinner(byte data) {
    return (data >> 1) & 1;//returns bit E
}

byte getDiamond(byte data) {
    return (data) & 1;//returns bit F
}
