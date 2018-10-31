#include "BallTracker.h"

BallTracker::BallTracker(SpellBook *spellBook)
    : InnerModule(spellBook)
{
    // Este é o contrutor e será executado quando o programa for aberto
    // Aqui é um bom lugar para setar variáveis iniciais ou qualquer configuração básica inicial,
    // como criar novos objetos ou definir parâmetros
    // Não use o SpellBook nesse ponto pois ele não será salvo
}

BallTracker::~BallTracker()
{
    // Este é o destrutor e será executado quando o programa for fechado
    // Aqui é um bom lugar para destruir objetos criados no módulo
}

void BallTracker::OnStart()
{
    // Este método é executado quando o módulo começa rodar
    // Aqui é um bom lugar para definir o código que precisa rodar na inicialização do módulo
    // aqui é possível utilizar o SpellBook
}

void BallTracker::OnStop()
{
    // Este método é chamado quando o módulo para de rodar
    // Aqui é um bom lugar para para rodar o código que precisa rodar quando o módulo parar
    // Aqui é possível utilizar o SpellBook
}

void BallTracker::Tick(float ellapsedTime, const SensorValues &sensor)
{
    // Este é o método principal do módulo, ele irá ser executado periodicamente
    // O código principal deve estar aqui
    // Não devem haver loops infinitos neste método pois ele já roda em um loop infinito
    // Aqui é possível utilizar o SpellBook
    // ellapsedTime é o tempo desde a ultima chamada deste método

    int ballX = spellBook->perception.vision.ball.ImageX;
    int ballY = spellBook->perception.vision.ball.ImageY;
    
    RelativeCoord rc;
    rc.fromPixel(ballX, ballY);
    
    CartesianCoord cc;
    rc.toCartesian(cc);

    cout << "Ball" << endl;
    cout << rc.getDistance() << ", " << Rad2Deg(rc.getYaw()) << "º, " << Rad2Deg(rc.getPitch()) << "º" << endl;
    cout << cc.getX() << ", " << cc.getY() << endl;

    // Distancia e ângulo
    // rc.getDistance() e rc.getYaw()

    // X e Y em relação ao robô
    // cc.getX() e cc.getY()    
}
