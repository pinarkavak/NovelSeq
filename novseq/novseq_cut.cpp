#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

void checkRunResult(int result)
{
				if (result != 0)
				{
								printf(">>>>>>\tCommand exited with error code %d. Exiting!\n");
								exit(result);
				}
}

void runCommand(char* command)
{
				printf("%s\r\n", command);
				int runResult = system(command);
				checkRunResult(runResult);
}

int main(int argc, char** argv)
{
				if (argc < 8)
				{
								printf("Not enough arguments passed. Usage: novsql_reza <dir> <dataset> <satRemChrs> <chroDir> <histogram> <maxNumOfReadsEachCluster> <maxInsertSize>  <minSupportLeft> [minSupportRight]\r\n");
								return -1;
				}

				char* dirFromInput = argv[1];
				char* datasetFromInput = argv[2];
				char* satRemChrs = argv[3];
				char* chroDir = argv[4];
				char* histogramFile = argv[5];
				char* maxNumOfReads = argv[6];
				int  maxIns = atoi(argv[7]);
				
				int minSupportLeft=atoi(argv[8]);
				int minSupportRight = minSupportLeft;
				if (argc == 10)
								minSupportRight = atoi(argv[9]);

				char command[1024];

				char dir[1024];
				strcpy(dir, dirFromInput);
				if (!(dir[strlen(dir) - 1] == '/'))
								strcat(dir, "/");

				char dataset[1024];
				strcpy(dataset, dir);
				strcat(dataset, datasetFromInput);

				printf("Dir:\t%s\n", dir);
				printf("Dataset:\t%s\n", dataset);
				printf("--\n\n");

				int runResult = 0;

				sprintf(command, "./sam2novelseq -f %s %s.type1 %s ", dataset, dataset, satRemChrs);
				runCommand(command);

				sprintf(command, "./parserQualWReadPrunWSatellite %s.type1 %s > %s.parsed", dataset, satRemChrs, dataset);
				runCommand(command);

				sprintf(command, "cut -f1,8 %s.parsed > %s.Name", dataset, dataset);
				runCommand(command);

				sprintf(command, "./unique %s.Name > %s.Name.Unique", dataset, dataset);
				runCommand(command);

				sprintf(command, "./sortString %s.Name.Unique > %s.Name.Unique.sort", dataset, dataset);
				runCommand(command);

				sprintf(command, "cut -f1,5,7,8 %s.parsed > %s.Name.Edit.Prob", dataset, dataset);
				runCommand(command);

				sprintf(command, "./calProbMinEditRead %s.Name.Unique.sort %s.Name.Edit.Prob > %s.Name.MinEdit.TotalProb", dataset, dataset, dataset);
				runCommand(command);

				sprintf(command, "./convertToInvLRProbMinEditDist %s.parsed %s.Name.MinEdit.TotalProb > %s.parsed.LR", dataset, dataset, dataset);
				runCommand(command);


				char cdir[1024];
				strcpy(cdir, chroDir);
				if (!(cdir[strlen(cdir) - 1] == '/'))
								strcat(cdir, "/");


				for (int chr = 1; chr < 24; chr++)
				{
								sprintf(command, "./OEASetsProb %schroName%d %s.parsed.LR %s.parsed.chr%d %d", cdir, chr, dataset, dataset, chr, maxIns);
								runCommand(command);
				}

				sprintf(command, "./OEASetsProb %schroNameX %s.parsed.LR %s.parsed.chrX %d", cdir, dataset, dataset, maxIns);
				runCommand(command);

				sprintf(command, "./OEASetsProb %s/chroNameY %s.parsed.LR %s.parsed.chrY %d", cdir, dataset, dataset, maxIns);
				runCommand(command);

				sprintf(command, "cat %s.parsed.chr? >> %s.parsed.AllChr", dataset, dataset);
				runCommand(command);

				sprintf(command, "cat %s.parsed.chr?? >> %s.parsed.AllChr", dataset, dataset);
				runCommand(command);

				sprintf(command, "./setCoverProbWeighted %s.Name.Unique.sort %s.parsed.AllChr %d %d > %s.clusters", dataset, dataset, minSupportLeft, minSupportRight, dataset);
				runCommand(command);
 
/*	
//				sprintf(command, "./type1to2convertor %s.clusters %s.all 35000011 > %s.type2", dataset, dataset, dataset, dataset);
//				runCommand(command);

	//			sprintf(command, "./oea.LocalAssembly %s %s.parsed.AllChr.type2 %s ", histogramFile, dataset, maxNumOfReads); //if you want to parrallel this, change here and add min and max cluster ids to this line
//				runCommand(command);
				


				//	printf("%s\r\n", command);
				//	runResult = system(command);
				//checkRunResult(runResult);

*/
				return 0;
}

