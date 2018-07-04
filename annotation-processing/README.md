This README describes how to infer process models from the annotated attack reports (logs)

1. Directory logs contains the annotations made by each annotator on the reports delivered by the experimental subjects. Logs are in TXT format and their name follows the general convention: <subject-id>_<annotator-id>.docx.txt (e.g., S01_PT.docx.txt).

2. Logs are converted to CSV files by running the Perl script convert-to-csv.prl on each log. The Shell script run.sh automates such process for all logs, grouping them into FAIL/SUCCESS; CLEAR(C)/SPLIT-MEDIUM(M)/SPLIT-SMALL(S). The resulting CSV files are named according to this convention: logs_<group-label>.csv.

3. The tool Disco (not provided) can now be executed on each CSV file. If executed with the threshold for activity frequency set to 20% and no threshold for path frequency, the same diagrams contained in the PDF files should be obtained (the name of PDF files follows the convention logs_<group-label>.pdf). 

