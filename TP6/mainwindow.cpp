#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Commande* commande, Filtre* filtre, Menu* menu, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    commande_(commande), filtre_(filtre), menu_(menu)
{
    ui->setupUi(this);
    setUI();
    connecterSignauxAuxSlots();
    mettreAJourPlatsFiltres();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUI(){
    setWindowTitle(WINDOW_TITLE);
    creerWidgets();
    designLayout();
}

void MainWindow::creerWidgets(){
    creerWidgetTypeMenu();
    creerWidgetConditionsAlimentaires();
    creerWidgetPlatsFiltres();
    creerWidgetCommande();
    creerWidgetBoutonsAjouterRetirer();
    creerWidgetPrix();
    creerWidgetCommander();
}

void MainWindow::creerWidgetTypeMenu() {
    widgetTypeMenu_ = new QComboBox(this);
    widgetTypeMenu_->addItem(MATIN);
    widgetTypeMenu_->addItem(MIDI);
    widgetTypeMenu_->addItem(SOIR);
}

void MainWindow::creerWidgetConditionsAlimentaires(){
    widgetPlatsBios_ =  new QCheckBox(BIOLOGIQUE,this);
    widgetPlatsVeges_ = new QCheckBox(VEGETARIEN, this);
}

void MainWindow::creerWidgetPlatsFiltres(){
    widgetPlatsFiltres_ = new QListWidget(this);
    widgetPlatsFiltres_->setSortingEnabled(true);
}

//TODO
void MainWindow::creerWidgetCommande() {
    widgetCommande_= new QListWidget(this);
    widgetCommande_->setSortingEnabled(true);


}

void MainWindow::creerWidgetBoutonsAjouterRetirer() {
    widgetAjouterPlat_ = new QPushButton(this);
    widgetAjouterPlat_->setText(AJOUTER_PLAT);
    widgetRetirerPlat_ = new QPushButton(this);
    widgetRetirerPlat_->setText(RETIRER_PLAT);
}

void MainWindow::creerWidgetPrix() {
    widgetPrix_ = new QLabel(this);
    widgetPrix_->setText(PRIX_STRING + "0" + DOLLAR_SIGN);
}

//TODO
void MainWindow::creerWidgetCommander() {
   widgetCommander_= new QPushButton(this) ;
   widgetCommander_->setText(COMMANDER);
}



//TODO
void MainWindow::designLayout() {

    // Titre de la fenêtre
    string title = "Faire une commande";
    setWindowTitle(title.c_str());

    // Section contenant les options de filtrage des plats
    QVBoxLayout* menu = new QVBoxLayout(this);
    menu->addWidget(widgetTypeMenu_);
    menu->addWidget(widgetPlatsBios_);
    menu->addWidget(widgetPlatsVeges_);

    // Option pour Ajouter et Retirer Plat
    QHBoxLayout* options = new QHBoxLayout(this);
    options->addWidget(widgetAjouterPlat_);
    options->addWidget(widgetRetirerPlat_);

    // Liste des plats filtrés
    QVBoxLayout* choixUsager = new QVBoxLayout(this);
    choixUsager->addWidget(widgetPlatsFiltres_);
    choixUsager->addLayout(options);
    QHBoxLayout* topBox = new QHBoxLayout(this);
    topBox->addLayout(menu);
    topBox->addLayout(choixUsager);

    // Trait horizontale
    QFrame* hLine = new QFrame();
    hLine->setFrameShape(QFrame::HLine);

    // Liste des plats contenus dans la commande
    QHBoxLayout* listeCommande = new QHBoxLayout(this);
    listeCommande->addWidget(widgetCommande_);

    // Prix actualisé de la commande
    QHBoxLayout* commandePlacee = new QHBoxLayout(this);
    commandePlacee->addWidget(widgetPrix_);
    commandePlacee->addWidget(widgetCommander_);

    QVBoxLayout* bottomBox = new QVBoxLayout(this);
    bottomBox->addLayout(listeCommande);
    bottomBox->addLayout(commandePlacee);

    QVBoxLayout* mainBox = new QVBoxLayout(this);
    mainBox->addLayout(topBox);
    mainBox->addWidget(hLine);
    mainBox->addLayout(bottomBox);
    QWidget* mainWidget = new QWidget();
    mainWidget->setLayout(mainBox);
    setCentralWidget(mainWidget);


}

/*
 * Connecte les actions de l'utilisateur sur l'interface aux modifications qui
 * doivent être faites sur le modèle
*/
void MainWindow::connecterSignauxAuxSlots() {
    // connecter checkboxes filtre bio et vege
    QObject::connect(widgetPlatsVeges_, SIGNAL(stateChanged(int)),
                     this, SLOT(clicOptionVege()));
    QObject::connect(widgetPlatsBios_, SIGNAL(stateChanged(int)),
                     this, SLOT(clicOptionBio()));

    //connecter menu defilant type plat
    QObject::connect(widgetTypeMenu_, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(choixTypeMenu(QString)));

    //TODO
    //connecter boutons ajouterPlat et retirerPlat
    QObject::connect(widgetAjouterPlat_,SIGNAL(clicked()),this,SLOT(insererPlatsChoisisDansCommande()));
     QObject::connect(widgetRetirerPlat_,SIGNAL(clicked()),this,SLOT(retirerPlatsChoisisDeCommande()));
    //connecter bouton commander
    QObject::connect(widgetCommander_, SIGNAL(clicked()),
                     this, SLOT(commander()));
}
void MainWindow::mettreAJourPlatsFiltres(){
    QStringList plats = filtre_->getNomPlatsFiltres();
    widgetPlatsFiltres_->clear();
    widgetPlatsFiltres_->addItems(plats);
}

void MainWindow::clicOptionBio(){
    filtre_->clicOptionBio();
}

void MainWindow::clicOptionVege(){
    filtre_->clicOptionVege();
}

void MainWindow::choixTypeMenu(QString typeMenu){
    filtre_->choixTypeMenu(qstringToTypeMenu(typeMenu));
}

void MainWindow::mettreAJourPlatsCommande(){
    widgetCommande_->clear();
    foreach (auto nomPlat, commande_->getCommande()) {
        widgetCommande_->addItem(nomPlat);
    }
}

//TODO
void MainWindow::insererPlatsChoisisDansCommande() {
    commande_->ajouterPlat(widgetPlatsFiltres_->currentItem()->text());

}

//TODO
void MainWindow::retirerPlatsChoisisDeCommande() {
    try {
          commande_->retirerPlat(widgetPlatsFiltres_->currentItem()->text());
      } catch (ErreurPlatIntrouvable erreur) {
          message(erreur.what());
      }
}

void MainWindow::mettreAJourPrix() {

    widgetPrix_->setText(PRIX_STRING +
                         QString::number(calculerPrixCommande()) +
                         DOLLAR_SIGN);
}

int MainWindow::calculerPrixCommande(){
    int prix = 0;
    try {
        foreach(auto nomPlatCommande, commande_->getCommande()){
            prix += menu_->getPlat(nomPlatCommande)->getPrix();
        }
    } catch (ErreurPlatIntrouvable erreur) {
        message(erreur.what());
    }
    return prix;
}

void MainWindow::message(QString erreur) {
    QMessageBox mbox;
    mbox.setText(erreur);
    mbox.exec();
}

void MainWindow::commander() {
    try {
        int prix = calculerPrixCommande();
        commande_->viderCommande();
        message(LABEL_COMMANDE_RECUE+ QString::number(prix) + DOLLAR_SIGN);
    } catch (ErreurCommandeVide erreur) {
        message(erreur.what());
    }
}
