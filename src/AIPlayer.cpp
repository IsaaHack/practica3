# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    // Tutorial
    /**
    switch (id)
    {
    case 0:
        thinkAleatorio(c_piece, id_piece, dice);
        break;
    
    case 1:
        thinkAleatorioMasInteligente(c_piece, id_piece, dice);
        break;

    case 2:
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
        break;

    case 3:
        thinkMejorOpcion(c_piece, id_piece, dice);
        break;
    }
    */

    
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion4);
            break;
        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;
        case 3:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion3);
            break;
        case 4:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;
    }
    cout << "Valor AlfaBeta: MIO " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

void AIPlayer::thinkAleatorio(color & c_piece, int & id_piece, int & dice) const{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color & c_piece, int & id_piece, int & dice) const{
    
    // El número de mi jugador actual.

    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento.

    vector<int> current_dices_con_especiales;

    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.

    vector<tuple<color, int>> current_pieces;

    // Obtengo el vector de dados que puedo usar para el movimiento.
    // En este caso elijo todos, tanto normales como especiales.
    // Importante: puedo acceder a solo dados normales o especiales por separado,
    // o a todos a la vez:
    // - actual->getAvailableNormalDices(player) -> solo dados normales
    // - actual->getAvailableSpecialDices(player) -> solo dados especiales
    // - actual->getAllAvailableDices(player) -> todos los dados

    // Importante 2: los "available" me dan los dados que puedo usar en el turno actual.
    // Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
    // Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
    // - actual->getNormalDices(player) -> todos los dados normales
    // - actual->getSpecialDices(player) -> todos los dados especiales
    // - actual->getAllDices(player) -> todos los dados

    current_dices_con_especiales = actual->getAllAvailableDices(player);


    // En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;
    for (int i = 0; i < current_dices_con_especiales.size(); i++){
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.

        current_pieces = actual->getAvailablePieces(player, current_dices_con_especiales[i]);
        
        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0){
            current_dices_que_pueden_mover_ficha.push_back(current_dices_con_especiales[i]);
        }
    }

    // Si no tengo ninún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).

    if (current_dices_que_pueden_mover_ficha.size() == 0){
        dice = current_dices_con_especiales[rand() % current_dices_con_especiales.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.

    } // En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
    else{
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.

        current_pieces = actual->getAvailablePieces(player, dice);

        // Muevo una ficha al azar de entre las que se pueden mover.

        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color & c_piece, int & id_piece, int & dice) const{
    // Elijo el dado haciendo lo mismo que el jugador anterior.

    thinkAleatorioMasInteligente(c_piece, id_piece, dice);

    // Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
    // Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
    // el dado especial a) la que esté más adelantada
    // (equivalentemente, la más cercana a la meta).

    int player = actual->getCurrentPlayerId();
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);

    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;

    for (int i = 0; i < current_pieces.size(); i++){
        // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.

        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        
        if (distancia_meta < min_distancia_meta){
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
    }

    // Si no he encontrado ninguna ficha, paso turno.
    if (id_ficha_mas_adelantada == -1){
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }// En caso contrario, moveré la ficha más adelantada.
    else{
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }

}

void AIPlayer::thinkMejorOpcion(color & c_piece, int & id_piece, int & dice) const{
    // Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.

    // Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
    // Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable
    // sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.

    ParchisBros hijos = actual->getChildren();
    bool me_quedo_con_esta_accion = false;

    // La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
    // Al hacer begin() accedemos al primer hijo de la rama,
    // y cada vez que hagamos ++it saltaremos al siguiente hijo.
    // Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.

    for(ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and !me_quedo_con_esta_accion; ++it){
        Parchis siguiente_hijo = *it; // Accedemos al tablero hijo con el operador de indirección.

        if(siguiente_hijo.isEatingMove() or // Si he comido ficha o …
            siguiente_hijo.isGoalMove() or // … una ficha ha llegado a la meta o …
            (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador) // … he ganado
            )
        {
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor(); // Guardo color de la ficha movida.
            id_piece = it.getMovedPieceId(); // Guardo id de la ficha movida.
            dice = it.getMovedDiceValue(); // Guardo número de dado movido.
        }

    }

    // Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
    // (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).
    // Si no, muevo la ficha más adelantada como antes.

    if(!me_quedo_con_esta_accion){
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
    }
}

double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const{
    if(profundidad == profundidad_max || actual.gameOver()){
        return heuristic(actual, jugador);
    }

    bool maximizar = actual.getCurrentPlayerId() == jugador;//Es el jugador que tiene el turno
    bool es_raiz = profundidad == 0;//Es el nodo raiz
    ParchisBros acciones = actual.getChildren();///Obtengo los hijos del nodo actual
    double valor;

    for(auto accion = acciones.begin(); accion != acciones.end(); ++accion){
        
        valor = Poda_AlfaBeta(*accion, jugador, profundidad+1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic);
        
        if(maximizar){//MAX
            if (valor > alpha){
                alpha = valor;

                if(es_raiz){
                    c_piece = accion.getMovedColor();
                    id_piece = accion.getMovedPieceId();
                    dice = accion.getMovedDiceValue();
                }
            }
            if(alpha >= beta){//Poda
                return beta;
            }
            
        }else{//MIN
            if (valor < beta){
                beta = valor;

                /**
                if(es_raiz){
                    c_piece = accion.getMovedColor();
                    id_piece = accion.getMovedPieceId();
                    dice = accion.getMovedDiceValue();
                }
                */
            }
            if(alpha >= beta){//Poda
                return alpha;
            }
        }
    }

    //Ha llegado al final del for, por lo que no ha podido podar
    if(maximizar){//MAX
        return alpha;
    }else{//MIN
        return beta;
    }
}

double AIPlayer::MiniMax(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double (*heuristic)(const Parchis &, int)) const{
    if(profundidad == profundidad_max || actual.gameOver()){
        return heuristic(actual, jugador);
    }

    double valor_nodo;//Valor del nodo actual
    bool maximizar = actual.getCurrentPlayerId() == jugador;//Si es el turno del jugador
    bool es_raiz = profundidad == 0;//Si es la raiz
    ParchisBros acciones = actual.getChildren();//Acciones posibles
    if(actual.getCurrentPlayerId() == jugador){//Si es el turno del jugador se busca el maximo
        valor_nodo = menosinf;
    }else{//Si es el turno del oponente se busca el minimo
        valor_nodo = masinf;
    }

    for(auto accion = acciones.begin(); accion != acciones.end(); ++accion){//Recorremos las acciones
        
        double valor = MiniMax(*accion, jugador, profundidad+1, profundidad_max, c_piece, id_piece, dice, heuristic);
        if(maximizar){//MAX
            if(valor_nodo < valor){
                valor_nodo = valor;

                if(es_raiz){
                    c_piece = accion.getMovedColor();
                    id_piece = accion.getMovedPieceId();
                    dice = accion.getMovedDiceValue();
                }
            }
        }else{//MIN
            if(valor_nodo > valor){
                valor_nodo = valor;

                /**
                if(es_raiz){
                    c_piece = accion.getMovedColor();
                    id_piece = accion.getMovedPieceId();
                    dice = accion.getMovedDiceValue();
                }
                */
            }
        }
    }

    return valor_nodo;//Devolvemos el valor del nodo maximo o minimo segun el turno
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::MiValoracion1(const Parchis &estado, int jugador){
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador){
        return gana;
    }else if (ganador == oponente){
        return pierde;
    }else{
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++){
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++){
                if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_jugador -= 5;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_jugador += 10;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_jugador += 15;
                }

                if(estado.isSafePiece(c, j)){
                    puntuacion_jugador += 2;
                }

                if(estado.goalBounce()){
                    puntuacion_jugador -= 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++){
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++){
                if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_oponente -= 5;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_oponente += 10;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_oponente += 15;
                }

                if(estado.isSafePiece(c, j)){
                    puntuacion_oponente += 5;
                }

                if(estado.goalBounce()){
                    puntuacion_oponente -= 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::MiValoracion2(const Parchis &estado, int jugador){
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador){
        return gana;
    }else if (ganador == oponente){
        return pierde;
    }if(estado.isEatingMove()){
        return -100;

    }else{
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);
        vector<int> my_dices = estado.getAllAvailableDices(jugador);
        vector<int> op_dices = estado.getAllAvailableDices(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++){
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++){
                if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_jugador -= 5;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_jugador += 10;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_jugador += 15;
                }

                if(estado.isSafePiece(c, j)){
                    puntuacion_jugador += 2;
                }

                if(estado.goalBounce()){
                    puntuacion_jugador -= 5;
                }

                puntuacion_jugador += 84 - estado.distanceToGoal(c, j);
            }
        }

        for(int i = 0; i < my_dices.size(); i++){
            if(my_dices[i] <= 20){
                puntuacion_jugador += my_dices[i];
            }else{
                switch (my_dices[i])
                {
                case star:
                    puntuacion_jugador += 30;
                    break;

                case boo:
                    puntuacion_jugador += 20;
                    break;

                case bullet:
                    puntuacion_jugador += 40;
                    break;

                case red_shell:
                    puntuacion_jugador += 20;
                    break;

                case blue_shell:
                    puntuacion_jugador += 50;
                    break;

                case mushroom:
                    puntuacion_jugador += 8;
                    break;

                case mega_mushroom:
                    puntuacion_jugador += 12;
                    break; 

                case shock:
                    puntuacion_jugador += 7;
                    break;

                case horn:
                    puntuacion_jugador += 15;
                    break;

                case banana:
                    puntuacion_jugador += 6;
                    break;
                
                default:
                    break;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++){
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++){
                if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_oponente -= 5;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_oponente += 10;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_oponente += 15;
                }

                if(estado.isSafePiece(c, j)){
                    puntuacion_oponente += 2;
                }

                if(estado.goalBounce()){
                    puntuacion_oponente -= 5;
                }

                puntuacion_oponente += 84 - estado.distanceToGoal(c, j);
            }
        }

        for(int i = 0; i < op_dices.size(); ++i){
            if(op_dices[i] <= 20){
                puntuacion_jugador += op_dices[i];
            }else{
                switch (op_dices[i])
                {
                case star:
                    puntuacion_oponente += 20;
                    break;

                case boo:
                    puntuacion_oponente += 12;
                    break;

                case bullet:
                    puntuacion_oponente += 40;
                    break;

                case red_shell:
                    puntuacion_oponente += 10;
                    break;

                case blue_shell:
                    puntuacion_oponente += 30;
                    break;

                case mushroom:
                    puntuacion_oponente += 8;
                    break;

                case mega_mushroom:
                    puntuacion_oponente += 12;
                    break; 

                case shock:
                    puntuacion_oponente += 7;
                    break;

                case horn:
                    puntuacion_oponente += 15;
                    break;

                case banana:
                    puntuacion_oponente += 6;
                    break;
                
                default:
                    break;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::MiValoracion3(const Parchis &estado, int jugador){
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador){
        return gana;
    }else if (ganador == oponente){
        return pierde;
    }if(estado.isEatingMove()){
        return -100;

    }else{
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);
        vector<int> my_dices = estado.getAllAvailableDices(jugador);
        vector<int> op_dices = estado.getAllAvailableDices(oponente);
        int mi_ficha_mas_adelantada = 0;
        int op_ficha_mas_adelantada = 0;

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        vector<int> distancias_color = {0,0};
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++){
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++){
                if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_jugador -= 20;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_jugador += 10;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_jugador += 20;
                }

                if(estado.isSafePiece(c, j)){
                    puntuacion_jugador += 5;
                }

                if(estado.goalBounce()){
                    puntuacion_jugador -= 5;
                }

                int distancia_ficha = 74 - estado.distanceToGoal(c, j);

                if(distancia_ficha > mi_ficha_mas_adelantada){
                    mi_ficha_mas_adelantada = distancia_ficha;
                }

                distancias_color[i] += distancia_ficha*distancia_ficha;
            }
        }

        if(distancias_color[0] <= distancias_color[1]){
            puntuacion_jugador += (int)(sqrt(distancias_color[1]));
        }else{
            puntuacion_jugador += (int)(sqrt(distancias_color[0]));
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        distancias_color = {0,0};
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++){
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++){
                if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_oponente -= 20;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_oponente += 10;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_oponente += 20;
                }

                if(estado.isSafePiece(c, j)){
                    puntuacion_oponente += 5;
                }

                if(estado.goalBounce()){
                    puntuacion_oponente -= 5;
                }

                int distancia_ficha = 74 - estado.distanceToGoal(c, j);

                if(distancia_ficha > op_ficha_mas_adelantada){
                    op_ficha_mas_adelantada = distancia_ficha;
                }

                distancias_color[i] += distancia_ficha*distancia_ficha;
            }
        }

        if(distancias_color[0] <= distancias_color[1]){
            puntuacion_oponente += (int)(sqrt(distancias_color[1]))*2;
            puntuacion_oponente += (int)sqrt(distancias_color[0]);
        }else{
            puntuacion_oponente += (int)(sqrt(distancias_color[0]))*2;
            puntuacion_oponente += (int)(sqrt(distancias_color[1]));
        }

        for(int i = 0; i < my_dices.size(); i++){
            if(my_dices[i] > 100)
            switch (my_dices[i])
            {
            case star:
                puntuacion_jugador += 30;
                break;

            case boo:
                puntuacion_jugador += 20;
                break;

            case bullet:
                puntuacion_jugador += 40;
                break;

            case red_shell:
                puntuacion_jugador += 20;
                break;

            case blue_shell:
                if(op_ficha_mas_adelantada > 50)
                    puntuacion_jugador += op_ficha_mas_adelantada;
                else
                    puntuacion_jugador += 10;
                break;

            case mushroom:
                puntuacion_jugador += 8;
                break;

            case mega_mushroom:
                puntuacion_jugador += 12;
                break; 

            case shock:
                puntuacion_jugador += 7;
                break;

            case horn:
                puntuacion_jugador += 15;
                break;

            case banana:
                puntuacion_jugador += 6;
                break;
            
            default:
                break;
            }
            
        }

        for(int i = 0; i < op_dices.size(); ++i){
            if(op_dices[i] > 100)
                switch (op_dices[i])
                {
                case star:
                    puntuacion_oponente += 30;
                    break;

                case boo:
                    puntuacion_oponente += 20;
                    break;

                case bullet:
                    puntuacion_oponente += 40;
                    break;

                case red_shell:
                    puntuacion_oponente += 20;
                    break;

                case blue_shell:
                    if(mi_ficha_mas_adelantada > 50)
                        puntuacion_oponente += mi_ficha_mas_adelantada;
                    else
                        puntuacion_oponente += 10;
                    break;

                case mushroom:
                    puntuacion_oponente += 8;
                    break;

                case mega_mushroom:
                    puntuacion_oponente += 12;
                    break; 

                case shock:
                    puntuacion_oponente += 7;
                    break;

                case horn:
                    puntuacion_oponente += 15;
                    break;

                case banana:
                    puntuacion_oponente += 6;
                    break;
                
                default:
                    break;
                }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

int recorrerFichas(const Parchis &estado, const vector<color> &fichas, const pair<color, int> &ficha_comida, int &ficha_adelantada, vector<unsigned int> &distancias_al_cuadrado){
    int puntuacion_jugador = 0;
    // Recorro colores de mi jugador.
    for (int i = 0; i < fichas.size(); i++){
        color c = fichas[i];
        // Recorro las fichas de ese color.
        for (int j = 0; j < num_pieces; j++){

            auto ficha = estado.getBoard().getPiece(c, j);//Ficha
            auto box = ficha.get_box();//Casilla en la que esta la ficha
            auto type = box.type;//Tipo de casilla en la que esta la ficha

            if(type == home){//Esta en casa
                puntuacion_jugador -= 20;
            }else if(type == final_queue){//Esta en la cola final
                puntuacion_jugador += 10;
            }else if(type == goal){//Esta en la meta
                puntuacion_jugador += 20;
            }

            if(estado.isSafePiece(c, j)){//Esta en una casilla segura
                puntuacion_jugador += 5;
            }

            if(estado.isWall(box)){//Esta haciendo barrera
                puntuacion_jugador += 5;
            }else{//Compruebo si hay fichas del mismo color en la misma casilla
                auto estadoBox = estado.boxState(box);
                if(estadoBox.size() > 1){//Hay mas de una ficha en la casilla
                    for(auto it = estadoBox.begin(); it != estadoBox.end(); ++it){
                        if(it->first == c && it->second != j){//Hay fichas del mismo color en la misma casilla
                            puntuacion_jugador += 2;
                        }
                    }
                }
            }

            if(estado.isMegaWall(box) && type == normal){//Esta haciendo barrera grande
                puntuacion_jugador += 2;
            }

            if(estado.goalBounce()){//Esta haciendo rebote
                puntuacion_jugador -= 5;
            }

            if(ficha_comida == pair<color, int>{c, j}){//Es la ficha comida
                puntuacion_jugador -= 10;
            }

            int distancia_ficha = 74 - estado.distanceToGoal(c, j);//Distancia recorrida por la ficha

            if(distancia_ficha > ficha_adelantada){//Es la ficha mas adelantada
                ficha_adelantada = distancia_ficha;
            }

            distancias_al_cuadrado[i] += distancia_ficha*distancia_ficha;//Sumo la distancia al cuadrado de la ficha
        }
    }

    return puntuacion_jugador;
}

int MiValoracionFichasEspeciales(const vector<int> &my_dices, const int &op_ficha_mas_adelantada){
    int puntuacion_jugador = 0;

    for(int i = 0; i < my_dices.size(); i++){
        switch (my_dices[i])
        {
        case star:
            puntuacion_jugador += 30;
            break;

        case boo:
            puntuacion_jugador += 20;
            break;

        case bullet:
            puntuacion_jugador += 40;
            break;

        case red_shell:
            puntuacion_jugador += 20;
            break;

        case blue_shell:
            if(op_ficha_mas_adelantada > 50)
                puntuacion_jugador += op_ficha_mas_adelantada;
            else
                puntuacion_jugador += 10;
            break;

        case mushroom:
            puntuacion_jugador += 8;
            break;

        case mega_mushroom:
            puntuacion_jugador += 12;
            break; 

        case shock:
            puntuacion_jugador += 7;
            break;

        case horn:
            puntuacion_jugador += 15;
            break;

        case banana:
            puntuacion_jugador += 6;
            break;
        
        default:
            break;
        }
        
    }

    return puntuacion_jugador;
}

double AIPlayer::MiValoracion4(const Parchis &estado, int jugador){
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador){
        return gana;
    }else if (ganador == oponente){
        return pierde;
    }else{
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);
        // Dados disponibles de mi jugador y del oponente
        vector<int> my_dices = estado.getAvailableSpecialDices(jugador);
        vector<int> op_dices = estado.getAvailableSpecialDices(oponente);
        auto piezaComida = estado.eatenPiece();// Ficha comida

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        int mi_ficha_mas_adelantada = 0;
        vector<unsigned int> distancias_color = {0,0};

        puntuacion_jugador += recorrerFichas(estado, my_colors, piezaComida, mi_ficha_mas_adelantada, distancias_color);

        // Distancia mi color más adelantado
        if(distancias_color[0] <= distancias_color[1]){
            puntuacion_jugador += (int)(sqrt(distancias_color[1]));
        }else{
            puntuacion_jugador += (int)(sqrt(distancias_color[0]));
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        int op_ficha_mas_adelantada = 0;
        distancias_color = {0,0};

        puntuacion_oponente += recorrerFichas(estado, op_colors, piezaComida, op_ficha_mas_adelantada, distancias_color);

        // Distancia oponente
        if(distancias_color[0] <= distancias_color[1]){
            puntuacion_oponente += (int)(sqrt(distancias_color[1]))*2;
            puntuacion_oponente += (int)sqrt(distancias_color[0]);
        }else{
            puntuacion_oponente += (int)(sqrt(distancias_color[0]))*2;
            puntuacion_oponente += (int)(sqrt(distancias_color[1]));
        }

        // Valoración de las fichas especiales
        puntuacion_jugador += MiValoracionFichasEspeciales(my_dices, op_ficha_mas_adelantada);
        puntuacion_oponente += MiValoracionFichasEspeciales(op_dices, mi_ficha_mas_adelantada);

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}
