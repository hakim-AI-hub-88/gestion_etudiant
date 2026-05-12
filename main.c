#include <mysql.h>
#include <gtk/gtk.h>
#include <stdio.h>



// Déclaration des fonctions
static void afficher_resultats_recherche(GtkWidget *fenetre_parent, const char *texte_recherche);
static void afficher_table(GtkWidget *parent_box);
static void sur_clic_bouton_recherche(GtkButton *button, gpointer user_data);
static void activer(GtkApplication *app, gpointer user_data);
static void actualiser_table(GtkWidget *widget, gpointer user_data);
// Fonction pour traiter le texte saisi par l'utilisateur
static void sur_clic_bouton_recherche(GtkButton *button, gpointer user_data) {
    GtkWidget *champ_recherche = GTK_WIDGET(user_data);
    const char *texte_recherche = gtk_entry_get_text(GTK_ENTRY(champ_recherche));

    // Vérifier si le texte est vide ou non
    if (texte_recherche == NULL || *texte_recherche == '\0') {
        g_print("Veuillez entrer un terme de recherche.\n");
        return;
    }

    // Ouvrir une fenêtre pour afficher les résultats de la recherche
    afficher_resultats_recherche(NULL, texte_recherche);
}

static void afficher_resultats_recherche(GtkWidget *fenetre_parent, const char *texte_recherche) {
    // Si le texte est vide, pas besoin de faire la recherche
    if (texte_recherche == NULL || *texte_recherche == '\0') {
        g_print("Le terme de recherche est vide.\n");
        return;
    }

    // Connexion à la base de données
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char requete[512];

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, "localhost", "root", "", "project_school", 3306, NULL, 0)) {
        g_print("Échec de la connexion à la base de données: %s\n", mysql_error(conn));
        return;
    }

    // Exécution de la requête de recherche
    snprintf(requete, sizeof(requete),
             "SELECT CNE, Nom, Prenom, Filiere, Date_Naissance, Email_Acadymique, Adresse "
             "FROM etudiants WHERE Nom LIKE '%%%s%%' OR Adresse LIKE '%%%s%%'OR Filiere like '%%%s%%' OR Prenom LIKE '%%%s%%' OR CNE LIKE '%%%s%%'",
             texte_recherche,texte_recherche ,texte_recherche,texte_recherche, texte_recherche);

    if (mysql_query(conn, requete)) {
        g_print("Échec de l'exécution de la requête: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        g_print("Échec de la récupération des résultats: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    // Créer une nouvelle fenêtre pour afficher les résultats
    GtkWidget *fenetre_resultats = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fenetre_resultats), "Résultats de la Recherche");
    gtk_window_set_default_size(GTK_WINDOW(fenetre_resultats), 600, 400);
// create box GTK_ORIENTATION_VERTICAL
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(fenetre_resultats), box);

    // Créer le modèle de données
    GtkListStore *list_store = gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
//gtk_tree_view_new  pour afficher les donnee en tab
    GtkWidget *vue_arbre = gtk_tree_view_new();
    gtk_box_pack_start(GTK_BOX(box), vue_arbre, TRUE, TRUE, 0);

    // Ajouter des colonnes pour afficher les données dans le tableau
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "CNE", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "Nom", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "Prenom", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "Filiere", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "Date de naissance", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "Email Académique", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(vue_arbre), -1, "Adresse", renderer, "text", 6, NULL);

    // Remplir le modèle avec les données de la base de données -1 pour the function mysql_fetch_row(res)) this is the end
    GtkTreeIter iter;
    while ((row = mysql_fetch_row(res)) != NULL) {
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           0, row[0],
                           1, row[1],
                           2, row[2],
                           3, row[3],
                           4, row[4],
                           5, row[5],
                           6, row[6],
                           -1);
    }
// les donnees est existe en list_store pou afficher en vue_arbre
    gtk_tree_view_set_model(GTK_TREE_VIEW(vue_arbre), GTK_TREE_MODEL(list_store));
    g_object_unref(list_store);

    // Fermer la connexion à la base de données ?? g_object_unref(list_store);pour supprimer
    mysql_free_result(res);
    mysql_close(conn);

    gtk_widget_show_all(fenetre_resultats);
}

// Déclaration des fonctions
static void afficher_resultats_recherche(GtkWidget *fenetre_parent, const char *texte_recherche);
static void afficher_table(GtkWidget *parent_box);
static void sur_clic_bouton_recherche(GtkButton *button, gpointer user_data);
static void activer(GtkApplication *app, gpointer user_data);

// Function to handle the submit button click
static void on_submit_student_modifier(GtkWidget *button, gpointer user_data) {
    GtkWidget *ajouter_window = GTK_WIDGET(user_data);

    // Retrieve the input fields
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(ajouter_window));
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));

    const char *cne = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 0)));
    const char *nom = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 1)));
    const char *prenom = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 2)));
    const char *filiere = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 3)));
    const char *date_naissance = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 4)));
    const char *email = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 5)));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 6)));

    // Connect to MySQL database
    MYSQL *conn;
    char query[512];

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, "localhost", "root", "", "project_school", 3306, NULL, 0)) {
        g_print("Database connection failed: %s\n", mysql_error(conn));
        return;
    }

     // Construct the SQL INSERT query
    snprintf(query, sizeof(query),
             "UPDATE etudiants SET nom ='%s',prenom ='%s',filiere='%s', date_naissance='%s', Email_Acadymique='%s',adresse='%s' where CNE='%s';",
              nom,prenom,filiere,date_naissance,email,adresse,cne);

    // Execute the query
    if (mysql_query(conn, query)) {
        g_print("Query execution failed: %s\n", mysql_error(conn));
    } else {
        g_print("Student modifier successfully!??\n");
    }

    // Close the connection
    mysql_close(conn);

    // Close the window after submission
    gtk_widget_destroy(ajouter_window);
}

// Function to open the "Ajouter une étudiant" window requperer les les donnes saisir
static void open_modifier_etudiant_window(GtkWidget *button, gpointer user_data) {
    // Create a new GTK window for adding student
    GtkWidget *ajouter_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ajouter_window), "modifier les information d'un etudiant");
    gtk_window_set_default_size(GTK_WINDOW(ajouter_window), 400, 300);

    // Create a vertical box to arrange widgets
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(ajouter_window), box);

    // Add input fields for the student attributes
    GtkWidget *cne_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(cne_entry), "CNE :");
    gtk_box_pack_start(GTK_BOX(box), cne_entry, TRUE, TRUE, 5);

    GtkWidget *nom_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(nom_entry), "Nom");
    gtk_box_pack_start(GTK_BOX(box), nom_entry, TRUE, TRUE, 5);

    GtkWidget *prenom_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(prenom_entry), "Prénom");
    gtk_box_pack_start(GTK_BOX(box), prenom_entry, TRUE, TRUE, 5);

    GtkWidget *filiere_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(filiere_entry), "Filière");
    gtk_box_pack_start(GTK_BOX(box), filiere_entry, TRUE, TRUE, 5);

    GtkWidget *date_naissance_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(date_naissance_entry), "Date de naissance (YYYY-MM-DD)");
    gtk_box_pack_start(GTK_BOX(box), date_naissance_entry, TRUE, TRUE, 5);

    GtkWidget *email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Email académique");
    gtk_box_pack_start(GTK_BOX(box), email_entry, TRUE, TRUE, 5);

    GtkWidget *adresse_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(adresse_entry), "Adresse");
    gtk_box_pack_start(GTK_BOX(box), adresse_entry, TRUE, TRUE, 5);

    // Add a submit button
    GtkWidget *submit_button = gtk_button_new_with_label("modifier");
    gtk_box_pack_start(GTK_BOX(box), submit_button, FALSE, FALSE, 10);

    // Signal to handle the "Ajouter" button click
    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit_student_modifier), ajouter_window);

    // Show all widgets in the window
    gtk_widget_show_all(ajouter_window);
}



//---------------------------------------------------------------------------------

// supp sql
static void sql_supprimer(GtkWidget *button, gpointer user_data) {
    GtkWidget *ajouter_window = GTK_WIDGET(user_data);

    // Retrieve the input fields
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(ajouter_window));
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));
	const char *cne = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 1)));
    // Connect to MySQL database
    MYSQL *conn;
    char query[512];
    conn = mysql_init(NULL);
    if (!conn) {
        g_print("Error initializing MySQL connection.\n");
        return;
    }
	if (!mysql_real_connect(conn, "localhost", "root", "", "project_school", 3306, NULL, 0)) {
        g_print("Failed to connect to MySQL: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

     // Construct the SQL INSERT query
    snprintf(query, sizeof(query)," DELETE FROM etudiants WHERE CNE = '%s'", cne);
    // Execute the query
    if (mysql_query(conn, query)) {
        g_print("Failed to execute DELETE query: %s\n", mysql_error(conn));
    } else {
        g_print("Student successfully deleted!\n");
    }

    // Close the connection
    mysql_close(conn);
    // CLOSE WINDOW DROP
    gtk_widget_destroy(ajouter_window);


}

// supprimer
static void ovrir_fenetre_supp(GtkWidget *button, gpointer user_data) {
    // Create a new GTK window for adding student
    GtkWidget *ajouter_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ajouter_window), "supprimer");
    gtk_window_set_default_size(GTK_WINDOW(ajouter_window), 300, 140);
    // Create a vertical box to arrange widgets
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(ajouter_window), box);

    GtkWidget *cne_label = gtk_label_new("CNE :");
    gtk_box_pack_start(GTK_BOX(box), cne_label, FALSE, FALSE, 5);

    GtkWidget *cne_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(cne_entry), "CNE");
    gtk_box_pack_start(GTK_BOX(box), cne_entry, FALSE, FALSE, 5);
     // Add a submit button
    GtkWidget *submit_button = gtk_button_new_with_label("supprimer");
    gtk_box_pack_start(GTK_BOX(box), submit_button, FALSE, FALSE, 10);

    g_signal_connect(submit_button, "clicked", G_CALLBACK(sql_supprimer), ajouter_window);

    // Show all widgets in the window
    gtk_widget_show_all(ajouter_window);
}

// Function to handle the submit button click (declare it before using it)
// static void on_submit_student(GtkWidget *button, gpointer user_data);



// Function to handle the submit button click
static void on_submit_student(GtkWidget *button, gpointer user_data) {
    GtkWidget *ajouter_window = GTK_WIDGET(user_data);

    // Retrieve the input fields
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(ajouter_window));
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));

    const char *cne = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 0)));
    const char *nom = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 1)));
    const char *prenom = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 2)));
    const char *filiere = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 3)));
    const char *date_naissance = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 4)));
    const char *email = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 5)));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(g_list_nth_data(children, 6)));

    // Connect to MySQL database
    MYSQL *conn;
    char query[512];

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, "localhost", "root", "", "project_school", 3306, NULL, 0)) {
        g_print("Database connection failed: %s\n", mysql_error(conn));
        return;
    }

    // Construct the SQL INSERT query
    snprintf(query, sizeof(query),
             "INSERT INTO etudiants (CNE, Nom, Prenom, Filiere, Date_Naissance, Email_Acadymique, Adresse) "
             "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s')",
             cne, nom, prenom, filiere, date_naissance, email, adresse);

    // Execute the query
    if (mysql_query(conn, query)) {
        g_print("Query execution failed: %s\n", mysql_error(conn));
    } else {
        g_print("Student added successfully!\n");
    }

    // Close the connection
    mysql_close(conn);

    // Close the window after submission
    gtk_widget_destroy(ajouter_window);
}

// Function to open the "Ajouter une étudiant" window
static void open_ajouter_etudiant_window(GtkWidget *button, gpointer user_data) {
    // Create a new GTK window for adding student
    GtkWidget *ajouter_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ajouter_window), "Ajouter un étudiant");
    gtk_window_set_default_size(GTK_WINDOW(ajouter_window), 400, 300);

    // Create a vertical box to arrange widgets
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(ajouter_window), box);

    // Add input fields for the student attributes
    GtkWidget *cne_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(cne_entry), "CNE");
    gtk_box_pack_start(GTK_BOX(box), cne_entry, TRUE, TRUE, 5);

    GtkWidget *nom_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(nom_entry), "Nom");
    gtk_box_pack_start(GTK_BOX(box), nom_entry, TRUE, TRUE, 5);

    GtkWidget *prenom_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(prenom_entry), "Prénom");
    gtk_box_pack_start(GTK_BOX(box), prenom_entry, TRUE, TRUE, 5);

    GtkWidget *filiere_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(filiere_entry), "Filière");
    gtk_box_pack_start(GTK_BOX(box), filiere_entry, TRUE, TRUE, 5);

    GtkWidget *date_naissance_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(date_naissance_entry), "Date de naissance (YYYY-MM-DD)");
    gtk_box_pack_start(GTK_BOX(box), date_naissance_entry, TRUE, TRUE, 5);

    GtkWidget *email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Email académique");
    gtk_box_pack_start(GTK_BOX(box), email_entry, TRUE, TRUE, 5);

    GtkWidget *adresse_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(adresse_entry), "Adresse");
    gtk_box_pack_start(GTK_BOX(box), adresse_entry, TRUE, TRUE, 5);

    // Add a submit button
    GtkWidget *submit_button = gtk_button_new_with_label("Ajouter");
    gtk_box_pack_start(GTK_BOX(box), submit_button, FALSE, FALSE, 10);

    // Signal to handle the "Ajouter" button click
    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit_student), ajouter_window);

    // Show all widgets in the window
    gtk_widget_show_all(ajouter_window);
}



// Function to display the table (as in your existing code)
static void afficher_table(GtkWidget *parent_box) {
    //
    if (!GTK_IS_BOX(parent_box)) {
        g_print("nn\n");
        return;
    }

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    char *server = "localhost";
    char *user = "root";
    char *password = "";  // Password
    char *database = "project_school"; // Database name
    char *query = "SELECT * FROM etudiants ORDER BY nom ASC"; // Query to fetch data

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server, user, password, database, 3306, NULL, 0)) {
        g_print("Échec de la connexion: %s\n", mysql_error(conn));
        return;
    }

    if (mysql_query(conn, query)) {
        g_print("Échec de l'exécution de la requête: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        g_print("Aucune donnée trouvée: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    GList *children = gtk_container_get_children(GTK_CONTAINER(parent_box));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_widget_set_hexpand(scrolled_window, TRUE);

    GtkWidget *tree_view = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

    GtkListStore *list_store = gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "CNE", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Nom", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Prenom", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Filiere", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Date de naissance", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Email Académique", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Adresse", renderer, "text", 6, NULL);

    while ((row = mysql_fetch_row(res)) != NULL) {
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           0, row[0],
                           1, row[1],
                           2, row[2],
                           3, row[3],
                           4, row[4],
                           5, row[5],
                           6, row[6],
                           -1);
    }
//
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(list_store));
    g_object_unref(list_store);
//
    mysql_free_result(res);
    mysql_close(conn);
//
    gtk_box_pack_start(GTK_BOX(parent_box), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show_all(parent_box);
}
static void actualiser_table(GtkWidget *widget, gpointer user_data) {
    GtkWidget *parent_box = GTK_WIDGET(user_data);

    if (!GTK_IS_BOX(parent_box)) {
        g_print("Erreur: L'élément passé n'est pas un GtkBox.\n");
        return;
    }

    GList *children = gtk_container_get_children(GTK_CONTAINER(parent_box));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
            // delete les information en table
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    afficher_table(parent_box);
    g_print("Tableau actualisé avec succès.\n");
}

// Function to activate GTK interface
static void activate(GtkApplication *app, gpointer user_data) {
    const char *css =
        "window {"
        "    background-color: #f79e9e;"
        "    color: #333333;"
        "    padding: 10px;"
        "}"

        "button {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 5px;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    transition: background-color 0.3s;"
        "}"

        "button:hover {"
        "    background-color: #45A049;"
        "}"

        "label, entry, treeview {"
        "    color: #333333;"
        "}"
        "treeview row {"
        "    background-color: #F9F9F9;"
        "}"

        "treeview row:hover {"
        "    background-color: #E0E0E0;"
        "}"

        "entry {"
        "    background-color: #FFFFFF;"
        "    border: 1px solid #CCCCCC;"
        "    border-radius: 4px;"
        "    padding: 5px;"
        "}"
;

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


    //+++++++++++++++++++++++++
    GtkWidget *window;
    GtkWidget *main_box; // Main box
    GtkWidget *left_box;  // Box for displaying the table
    GtkWidget *right_box; // Box for the button

    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ADMINISTRATION ESTK");
    gtk_window_set_default_size(GTK_WINDOW(window), 1400, 800);

    // Set up the main box layout 3amodi
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Set up the horizontal box for the layout (buttons on the right, table on the left)
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), hbox, TRUE, TRUE, 0);

    // Add the left box for the table
    left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(hbox), left_box, TRUE, TRUE, 0);

    // Add the right box for buttons
    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(hbox), right_box, FALSE, FALSE, 0);
      // Ajouter un champ de recherche et un bouton de recherche
    GtkWidget *box_recherche = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);  // Modifier l'orientation à verticale
    gtk_box_pack_start(GTK_BOX(right_box), box_recherche, FALSE, FALSE, 0);

    // Ajouter un champ de saisie
    GtkWidget *champ_recherche = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_recherche), champ_recherche, FALSE, FALSE, 0);
     // Ajouter un bouton de recherche
    GtkWidget *bouton_recherche = gtk_button_new_with_label("RECHERCHE");
    gtk_box_pack_start(GTK_BOX(box_recherche), bouton_recherche, FALSE, FALSE, 0);
    // Add buttons to the right box
    GtkWidget *bouton_actualiser = gtk_button_new_with_label("Mettre à jour la liste des étudiants");
    gtk_box_pack_start(GTK_BOX(right_box), bouton_actualiser, FALSE, FALSE, 0);
    GtkWidget *ajouter_une_etudiant = gtk_button_new_with_label("Ajouter une étudiant");
    gtk_box_pack_start(GTK_BOX(right_box), ajouter_une_etudiant, FALSE, FALSE, 0);
    GtkWidget *supprimer_une_etudiant = gtk_button_new_with_label("Supprimer une étudiant");
    gtk_box_pack_start(GTK_BOX(right_box), supprimer_une_etudiant, FALSE, FALSE, 0);
    GtkWidget *modifier_les_info_etudiant = gtk_button_new_with_label("Modifier les informations");
    gtk_box_pack_start(GTK_BOX(right_box), modifier_les_info_etudiant, FALSE, FALSE, 0);


    // COMNECT LA BOUTUNE DE MITRE A JOUR

    g_signal_connect(bouton_actualiser, "clicked", G_CALLBACK(actualiser_table), left_box);
    // Display the table in the left box
    afficher_table(left_box);
    // Connecter le bouton de recherche à la fonction de recherche
    g_signal_connect(bouton_recherche, "clicked", G_CALLBACK(sur_clic_bouton_recherche), champ_recherche);

    // Connect the "Refrech Etudiant" button to reload the table
    g_signal_connect(bouton_actualiser, "clicked", G_CALLBACK(afficher_table), left_box);

    // Connect the "Ajouter une étudiant" button to open the new student window
    g_signal_connect(ajouter_une_etudiant, "clicked", G_CALLBACK(open_ajouter_etudiant_window), NULL);
    //SUPPRIMER
    g_signal_connect(supprimer_une_etudiant, "clicked", G_CALLBACK(ovrir_fenetre_supp), NULL);
    // modifier
    g_signal_connect(modifier_les_info_etudiant, "clicked", G_CALLBACK(open_modifier_etudiant_window), NULL);


    // Show all components
    gtk_widget_show_all(window);
}



int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.student_app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
