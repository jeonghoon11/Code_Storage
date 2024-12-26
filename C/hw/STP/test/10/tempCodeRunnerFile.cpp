void replaceLabel(FILE* inputFile, FILE* outFile) {//out.txt에 저장하는 부분
//     char buffer[SIZE]; 
//     while (fgets(buffer, sizeof(buffer), inputFile)) {
//         char* token = strtok(buffer, "\t\n,+[] "); 
//         while (token != NULL) {
//             if (token[strlen(token) - 1] == ':') {
//                 token[strlen(token) - 1] = '\0';
//             }

//             if (labelMap.find(token) != labelMap.end()) {
//                 int line_number = labelMap[token];
//                 fprintf(outFile, "%d ", line_number);  
//             }
//             else {
//                 fprintf(outFile, "%s ", token);  
//             }
//             token = strtok(NULL, "\t\n,+[] "); 
//         }
//         // fprintf(outFile, "\n");
//     }
// }