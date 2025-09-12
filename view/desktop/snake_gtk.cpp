#include "snake_gtk.h"
#include "../../common/snake_consts.h"
#include <iostream>
#include <string>

GameViewGTK* game_view_instance = nullptr;

const GdkRGBA SNAKE_COLOR = {0.0, 1.0, 0.0, 1.0};      // Green
const GdkRGBA FOOD_COLOR = {1.0, 0.0, 0.0, 1.0};       // Red
const GdkRGBA INFO_COLOR = {1.0, 1.0, 0.0, 1.0};       // Yellow
const GdkRGBA BORDER_COLOR = {1.0, 1.0, 1.0, 1.0};     // White
const GdkRGBA BG_COLOR = {0.0, 0.0, 0.0, 1.0};         // Black

GameViewGTK::GameViewGTK() : game_over(false), paused(false) {
    game_view_instance = this;
}

GameViewGTK::~GameViewGTK() {
    cleanup();
}

bool GameViewGTK::initialize(int argc, char** argv) {
    gtk_init(&argc, &argv);
    
    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Snake Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    // Connect destroy signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create main container
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Create drawing area for game field
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, FIELD_WIDTH * 20 + 40, FIELD_HEIGHT * 20 + 40);
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);
 
    auto draw_func = +[] (GtkWidget* widget, cairo_t* cr, gpointer data) -> gboolean {
        if (game_view_instance) {
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_paint(cr);
            
            game_view_instance->drawBorders(cr);
            game_view_instance->drawField(game_view_instance->current_game_info.field, cr);
            
            if (game_view_instance->game_over) {
                game_view_instance->showGameOver();
            } else if (game_view_instance->paused) {
                game_view_instance->showPause();
            }
        }
        return FALSE;
    };

    // Connect draw signal
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_func), NULL);
    
    // Create sidebar
    sidebar_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), sidebar_box, FALSE, FALSE, 0);
    gtk_widget_set_size_request(sidebar_box, 200, -1);
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    return true;
}

void GameViewGTK::render(const GameInfo& game_info) {
    current_game_info = game_info;
    game_over = false;
    paused = game_info.pause;
    
    drawSidebar(game_info);
    gtk_widget_queue_draw(drawing_area);
}

void GameViewGTK::drawField(const std::vector<std::vector<int>>& field, cairo_t* cr) {
    const int cell_size = 20;
    const int offset_x = 20;
    const int offset_y = 20;
    
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            int cell = field[y][x];
            int pos_x = offset_x + x * cell_size;
            int pos_y = offset_y + y * cell_size;
            
            if (cell == 1) { // Snake
                gdk_cairo_set_source_rgba(cr, &SNAKE_COLOR);
                cairo_rectangle(cr, pos_x, pos_y, cell_size, cell_size);
                cairo_fill(cr);
            } else if (cell == 2) { // Food
                gdk_cairo_set_source_rgba(cr, &FOOD_COLOR);
                cairo_arc(cr, pos_x + cell_size/2, pos_y + cell_size/2, cell_size/2, 0, 2 * M_PI);
                cairo_fill(cr);
            }
        }
    }
}

void GameViewGTK::drawSidebar(const GameInfo& game_info) {
    // Clear sidebar
    GList* children = gtk_container_get_children(GTK_CONTAINER(sidebar_box));
    for (GList* iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    // Create labels with styling
    auto create_label = [](const char* text, const GdkRGBA* color) {
        GtkWidget* label = gtk_label_new(text);
        PangoAttrList* attrs = pango_attr_list_new();
        pango_attr_list_insert(attrs, pango_attr_foreground_new(
            1, 1, 1));
        gtk_label_set_attributes(GTK_LABEL(label), attrs);
        pango_attr_list_unref(attrs);
        return label;
    };
    
    char buffer[100];
    
    std::snprintf(buffer, sizeof(buffer), "SCORE: %d", game_info.score);
    GtkWidget* score_label = create_label(buffer, &INFO_COLOR);
    
    std::snprintf(buffer, sizeof(buffer), "HIGH: %d", game_info.high_score);
    GtkWidget* high_score_label = create_label(buffer, &INFO_COLOR);
    
    std::snprintf(buffer, sizeof(buffer), "LEVEL: %d", game_info.level);
    GtkWidget* level_label = create_label(buffer, &INFO_COLOR);
    
    std::snprintf(buffer, sizeof(buffer), "SPEED: %d", game_info.speed);
    GtkWidget* speed_label = create_label(buffer, &INFO_COLOR);
    
    // Add labels to sidebar with spacing
    gtk_box_pack_start(GTK_BOX(sidebar_box), score_label, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(sidebar_box), high_score_label, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(sidebar_box), level_label, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(sidebar_box), speed_label, FALSE, FALSE, 20);
    
    gtk_widget_show_all(sidebar_box);
}

void GameViewGTK::drawBorders(cairo_t* cr) {
    const int cell_size = 20;
    const int offset_x = 20;
    const int offset_y = 20;
    const int border_width = 2;
    
    gdk_cairo_set_source_rgba(cr, &BORDER_COLOR);
    cairo_set_line_width(cr, border_width);
    
    // Outer border
    cairo_rectangle(cr, offset_x - border_width, offset_y - border_width, 
                   FIELD_WIDTH * cell_size + 2 * border_width, 
                   FIELD_HEIGHT * cell_size + 2 * border_width);
    cairo_stroke(cr);
}

void GameViewGTK::showGameOver() {
    cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(drawing_area));
    
    // Semi-transparent overlay
    cairo_set_source_rgba(cr, 0, 0, 0, 0.7);
    cairo_paint(cr);
    
    // Game over text
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    
    const char* text = "GAME OVER!";
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    
    int x = (FIELD_WIDTH * 20 + 40) / 2 - extents.width / 2;
    int y = (FIELD_HEIGHT * 20 + 40) / 2 - extents.height / 2;
    
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
    
    cairo_set_font_size(cr, 16);
    text = "Press START to restart";
    cairo_text_extents(cr, text, &extents);
    x = (FIELD_WIDTH * 20 + 40) / 2 - extents.width / 2;
    cairo_move_to(cr, x, y + 30);
    cairo_show_text(cr, text);
    
    cairo_destroy(cr);
}

void GameViewGTK::showPause() {
    cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(drawing_area));
    
    // Semi-transparent overlay
    cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
    cairo_paint(cr);
    
    // Pause text
    cairo_set_source_rgb(cr, 1, 1, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    
    const char* text = "PAUSED";
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    
    int x = (FIELD_WIDTH * 20 + 40) / 2 - extents.width / 2;
    int y = (FIELD_HEIGHT * 20 + 40) / 2 - extents.height / 2;
    
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
    
    cairo_destroy(cr);
}

void GameViewGTK::cleanup() {
    if (window) {
        gtk_widget_destroy(window);
    }
    game_view_instance = nullptr;
}
