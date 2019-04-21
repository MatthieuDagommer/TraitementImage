/**
 * \file main.c
 * \brief Programme de traitement d'image.
 * \author Matthieu Dagommer & Louis Petitdemange
 * \version 1.0
 * \date 14 décembre 2018
 *
 * Programme premettant d'appliquer un filtre anti bruit ainsi que de préparer l'image a une compression en affichant les rectangle sur l'image, en choisissant l'ecart type max entre chaque pixel, et affiche le taux de compression. Pour une image .pgm de type P5 en brut.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> /**< bibliotheque pour les entiers codés sur 8bits, la valeur des pixels est codé sur 8bits */
#include <math.h>



int nbRect = 0; /**< variable global pemettant de compter le nombre de rectangle total*/

/** \brief Structure comportant les caractéristiques de l'image
 * \struct img
 */
typedef struct
{
    char magicNbr[2];/**< Le nombre magique de notre image (P5) */
    int colonnes;/**< Nombre de colonne de l'image */
    int lignes;/**< Nombre de ligne de l'image */
    int maxValue;/**< valeur max du pixel */

} img;

/**
 * \fn uint8_t mediane(uint8_t filtre[8])
 * \brief Calcul de la médiane de 9 valeurs.
 * \param filtre[8] uint8_t tableau de 9 valeurs.
 * \return uint8_t revoi la 5 eme valeur du tableau trié.
 *
 */
uint8_t mediane(uint8_t filtre[8])
{
    uint8_t c,valeurMediane;
    int i,j;
    for(j=1; j<=8; j++) /**< tri à bulle */
    {
        for(i=0; i<8-1; i++)
        {
            if ( filtre[i] > filtre[i+1] )
            {
                c = filtre[i];
                filtre[i] = filtre[i+1];
                filtre[i+1] = c;
            }
        }
    }
    valeurMediane = filtre[4];
    return valeurMediane;
}


/** \fn  void filtreMedian(uint8_t **tableau, int colonne, int ligne, uint8_t **imageFinal)
 * \brief application du filtre médian
 * \param tableau uint8_t** Tableau comportant les pixels a traiter
 * \param colonne int nombre de colonne du tableau
 * \param ligne int nombre de ligne
 * \param imageFinal uint8_t** on rentre l'image traité dans ce nouveau tableau
 * \return void
 *
 */
void filtreMedian(uint8_t **tableau, int colonne, int ligne, uint8_t **imageFinal)
{
    int i,j,i2,j2,m=0;
    uint8_t filtre[8];

    for(i=1; i<ligne-1; i++)/**< parcours des lignes pour l'application du filtre (on exclu la premiere et la derniere)*/
    {
        for(j=1; j<colonne-1; j++)/**<parcours des colonnes pour l'application du filtre (on exclu la premiere et la derniere)*/
        {
            /** remplissage du tableau filtre afin d'y appliquer la fonction médiane*/
            m=0;
            for(i2=i-1; i2<=i+1 ; i2++) /**<  i2 valeur de l'indice de tableau*/
            {
                for(j2=j-1; j2<=j+1; j2++)/**< j2 indice grand tableau*/
                {
                    filtre[m] = tableau[i2][j2]; /**< remplissage de la grille 3*3 dans un tableau*/
                    m++;

                }
            }
            imageFinal[i-1][j-1] = mediane(filtre); /**< remplissage pixel par pixel du tableau final en prenant la médiane du tableau 3*3*/
        }
    }
}

/** \brief Fonction permettant de faire la moyenne d'un rectangle, compris dans l'image de départ en fonction des differents indices
 *
 * \param tab uint8_t** tableau de départ contenant les pixels
 * \param g int indice de la colonne la plus a gauche
 * \param d int indice de la colonne la plus a droite
 * \param h int indice de la ligne la plus en haut
 * \param b int indice de la ligne la plus en bas
 * \return uint8_t retour de la moyenne des valeurs de ce tableau
 *
 */
uint8_t moyenne(uint8_t **tab, int g, int d, int h, int b)
{
    int i, j,somme = 0;
    uint8_t moy;
    for(i=h; i<b; i++)
    {
        for(j=g; j<d; j++)
        {
            somme = somme + tab[i][j];
        }
    }
    moy = somme / ((d-g)*(b-h));
    return moy;
}

/** \brief Calcul de l'écart type d'un rectangle, compris dans l'image de départ en fonction des differents indices
 *
 * \param tab uint8_t** tableau de départ contenant les pixels
 * \param g int indice de la colonne la plus a gauche
 * \param d int indice de la colonne la plus a droite
 * \param h int indice de la ligne la plus en haut
 * \param b int indice de la ligne la plus en bas
 * \return float retourne l'écart type des valeurs du rectangle
 *
 */
float ecart(uint8_t **tab,int g, int d, int h, int b)
{
    int i, j ;
    float somme = 0;
    uint8_t moy;
    moy = moyenne(tab,g,d,h,b);
    for(i=h; i<b; i++)
    {
        for(j=g; j<d; j++)
        {
            somme = somme + (float)(tab[i][j] - moy)*(tab[i][j] - moy);
        }
    }

    somme = (somme /(i*j));
    somme = sqrt(somme);
    return somme;
}

/** \brief Fonction permettant la representation simplifié ainsi que l'affichage des rectangles en utilisant la recurivité.
 *
 * \param g int indice de la colonne la plus a gauche
 * \param d int indice de la colonne la plus a droite
 * \param h int indice de la ligne la plus en haut
 * \param b int indice de la ligne la plus en bas
 * \param tab uint8_t** tableau contenant tous les pixels de départ
 * \param colonne int nombre de colonne dans l'image
 * \param ligne int nombre de ligne dans l'image
 * \param delta float valeur rentré par l'utilisateur afin de fixer un ecart type maximal
 * \return void
 *
 */
void simple(int g,int d,int h,int b, uint8_t **tab,int colonne, int ligne, float delta)
{
    float EcartType;
    uint8_t moy;

    EcartType = ecart(tab,g,d,h,b);
    moy = moyenne(tab,g,d,h,b);
    int taille;/**< variable permettant de ne plus diviser les rectangle quand il y a moins de 4 pixels */
    taille = (d-g)*(b-h); /**< calcul de la taille du rectangle */


    if(EcartType > delta && taille>=4)
    {
        int Xmiddle,Ymiddle;
        Xmiddle = g+(d-g)/2; /**< la moitie du rectangle sur la largeur */
        Ymiddle = h+(b-h)/2; /**< la moitie du rectangle sur la hauteur  */
        simple(g,Xmiddle,h,Ymiddle,tab,colonne,ligne,delta);/**< on rappelle la fonction en coupant le rectangle en 4, on fait donc 4 appels, 1 pour chaque nouveau rectangle */
        simple(Xmiddle,d,h,Ymiddle,tab,colonne,ligne,delta);
        simple(g,Xmiddle,Ymiddle,b,tab,colonne,ligne,delta);
        simple(Xmiddle,d,Ymiddle,b,tab,colonne,ligne,delta);
    }
    else
    {
        nbRect++; /**< on incrémente le nombre de rectangle quand on a fini le decoupage */
        int i,j;
        for(i=h; i<b; i++)/**< boucle pour affecter la valeur moyenne au rectangle  */
        {
            for(j=g; j<d; j++)
            {
                tab[i][j] = moy;
                if(i==(b-1))/**< si le pixel est sur le bord droit ou bas de l'image on le met en noir pour afficher le bord des rectangles */
                {
                    tab[i][j]=0;
                }
                if(j==(d-1))
                {
                    tab[i][j]=0;
                }
            }
        }
    }
}


/** \brief Fonction main
 * Ouvre, ferme les fichiers. Alloue la memoire et la libère.
 * \return int
 *
 */
int main()
{
    FILE *ptr;
    img image;
    char name[20];
    printf("entrez le nom de votre image avec l'extension de fichier\n");
    scanf("%s", name);
    ptr=fopen(name, "rb");
    if (ptr == NULL)
    {
        printf("ERREUR");
        return 1;
    }
    float delta;
    printf("quel ecart type maximal voulez vous entre chaque pixel d'un rectangle a remplacer ?");
    scanf("%f", &delta);
    fscanf (ptr, "%s %d %d %d", &image.magicNbr, &image.colonnes, &image.lignes, &image.maxValue);
    printf ("type : %s\nColonnes : %d\nLigne : %d\nValeur max du pixel : %d\n",image.magicNbr, image.colonnes, image.lignes, image.maxValue);
    fseek (ptr, sizeof ( uint8_t ),SEEK_CUR );
    //--------------------REMPLIR MATRICE------------------------------------------
    uint8_t **pixel;
    int i;
    pixel = (uint8_t**)malloc(image.lignes * sizeof(uint8_t*));
    for (i=0; i<image.lignes; i++)
    {
        pixel[i] = (uint8_t*) malloc(image.colonnes * sizeof(uint8_t));
    }
    for (i=0; i<image.lignes; i++)
    {
        fread(pixel[i], 1, image.colonnes, ptr);
    }
    //-------------------APPEL DE LA FONCTION FILTRE---------------------------------
    uint8_t **imageFinal; /**< création de la place mémoire sous forme de matrice pour l'image finale */
    imageFinal = (uint8_t**)malloc((image.lignes-2) * sizeof(uint8_t*));
    for (i=0; i<(image.lignes-2); i++)
    {
        imageFinal[i] = (uint8_t*) malloc((image.colonnes-2)* sizeof(uint8_t));
    }
    filtreMedian(pixel,image.colonnes,image.lignes,imageFinal); /**< application du filtre a l'image de base, enregistré dans la nouvelle matrice*/
    printf("\n");
    //---------------------APPEL DE LA FONCTION RepresentationSimplifie----------------------------
    simple(0,image.colonnes,0,image.lignes,pixel,image.colonnes,image.lignes, delta);
    //----------------------NOUVELLE IMAGE CRÉATION FiltreBruit-----------------------------------
    FILE* nouvelleImage1;
    nouvelleImage1=fopen("filtreBruit.pgm", "wb"); /**< création du fichier de la nouvelle image*/
    if (nouvelleImage1 == NULL)
    {
        printf("ERREUR");
        return 1;
    }
    fprintf(nouvelleImage1,"%s\n",image.magicNbr); /**< écriture magic number dans la nouvelle image*/
    fprintf(nouvelleImage1,"%d ",image.colonnes-2); /**< écriture nombre de colonne*/
    fprintf(nouvelleImage1,"%d\n",image.lignes-2);/**< écriture nombre de ligne*/
    fprintf(nouvelleImage1,"%d\n",image.maxValue);/**< écriture nombre max des pixel*/
    for (i=0; i<image.lignes-2; i++)
    {
        fwrite(imageFinal[i], 1, image.colonnes-2, nouvelleImage1);/**< on ecrit chaque ligne de la matrice de la nouvelle image dans le fichier de la nouvelle image. Avec 2 colonnes en moins car on a suprimé les bordures */
    }
    //---------------------NOUVELLE IMAGE ImageSimplifie--------------------------------------------
    FILE* nouvelleImage;
    nouvelleImage=fopen("imageSimplifie.pgm", "wb");
    if (nouvelleImage == NULL)
    {
        printf("ERREUR");
        return 1;
    }
    fprintf(nouvelleImage,"%s\n",image.magicNbr);
    fprintf(nouvelleImage,"%d ",image.colonnes);
    fprintf(nouvelleImage,"%d\n",image.lignes);
    fprintf(nouvelleImage,"%d\n",image.maxValue);
    for (i=0; i<image.lignes; i++)
    {
        fwrite(pixel[i], 1, image.colonnes, nouvelleImage);
    }
    //----------------------FERMETURE ET LIBERATION MEMOIRE--------------------------
    for (i=0; i<image.lignes; i++)
    {
        free(pixel[i]);
    }
    free(pixel);

    for (i=0; i<image.lignes-2; i++)
    {
        free(imageFinal[i]);
    }
    free(imageFinal);
    fclose(nouvelleImage1);
    fclose(nouvelleImage);
    fclose (ptr);
    float txCompression;
    txCompression =(float) (image.colonnes * image.lignes) / nbRect; /**< calcul du taux de compression */
    printf("\nLe taux de compression est de : %f \n", txCompression);
    system("pause");

    return 0;
}
