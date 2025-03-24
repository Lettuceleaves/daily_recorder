#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DAYS 7

typedef struct {
    char date[11]; // YYYY-MM-DD
    int keyboard_presses;
    int mouse_clicks;
} UsageData;

void generate_html(const UsageData data[], int days) {
    FILE *html_file = fopen("usage_data.html", "w");
    if (!html_file) {
        perror("Failed to create HTML file");
        return;
    }

    // 写入HTML头部
    fprintf(html_file, "<!DOCTYPE html>\n");
    fprintf(html_file, "<html lang=\"en\">\n");
    fprintf(html_file, "<head>\n");
    fprintf(html_file, "    <meta charset=\"UTF-8\">\n");
    fprintf(html_file, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(html_file, "    <title>Usage Data</title>\n");
    fprintf(html_file, "    <style>\n");
    fprintf(html_file, "        body { font-family: Arial, sans-serif; }\n");
    fprintf(html_file, "        .chart { display: flex; flex-direction: column; }\n");
    fprintf(html_file, "        .bar { background-color: #4CAF50; margin: 5px 0; }\n");
    fprintf(html_file, "        .mouse-bar { background-color: #2196F3; }\n");
    fprintf(html_file, "    </style>\n");
    fprintf(html_file, "</head>\n");
    fprintf(html_file, "<body>\n");

    // 写入键盘按键数据
    fprintf(html_file, "    <h1>Keyboard Presses</h1>\n");
    fprintf(html_file, "    <div class=\"chart\">\n");
    for (int i = 0; i < days; i++) {
        fprintf(html_file, "        <div>%s</div>\n", data[i].date);
        fprintf(html_file, "        <div class=\"bar\" style=\"width: %dpx; height: 20px;\">%d</div>\n", 
                data[i].keyboard_presses / 10, data[i].keyboard_presses);
    }
    fprintf(html_file, "    </div>\n");

    // 写入鼠标点击数据
    fprintf(html_file, "    <h1>Mouse Clicks</h1>\n");
    fprintf(html_file, "    <div class=\"chart\">\n");
    for (int i = 0; i < days; i++) {
        fprintf(html_file, "        <div>%s</div>\n", data[i].date);
        fprintf(html_file, "        <div class=\"mouse-bar\" style=\"width: %dpx; height: 20px;\">%d</div>\n", 
                data[i].mouse_clicks, data[i].mouse_clicks);
    }
    fprintf(html_file, "    </div>\n");

    // 写入HTML尾部
    fprintf(html_file, "</body>\n");
    fprintf(html_file, "</html>\n");

    fclose(html_file);
    printf("HTML file generated: usage_data.html\n");
}

int main() {
    FILE *file = fopen("daily_input_log.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    UsageData data[DAYS] = {0};
    char line[100];
    int day_count = 0;

    while (day_count < DAYS && fgets(line, sizeof(line), file)) {
        char date[11];
        char time[9];
        int keyboard_presses, mouse_clicks;
        if (sscanf(line, "%10s %8s - Keyboard presses: %d, Mouse clicks: %d", date, time, &keyboard_presses, &mouse_clicks) == 4) {
            strcpy(data[day_count].date, date);
            data[day_count].keyboard_presses = keyboard_presses;
            data[day_count].mouse_clicks = mouse_clicks;
            day_count++;
        }
    }

    fclose(file);

    // Fill missing days with 0
    for (int i = day_count; i < DAYS; i++) {
        snprintf(data[i].date, 11, "2025-03-%02d", 23 - (DAYS - 1 - i));
        data[i].keyboard_presses = 0;
        data[i].mouse_clicks = 0;
    }

    generate_html(data, DAYS);

    return 0;
}
