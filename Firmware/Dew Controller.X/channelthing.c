#include <stdio.h>
#include <stdlib.h>

#define numCh 4

typedef struct {
	int phyCh;
	float maxCur;
	int DC;
	int newDC;
} tChData;

tChData channels[4] = {{0, 2.5,50},{1, 1.0,60},{2, 0.5,80},{3, 2.0,80}};
int grpA[numCh], grpB[numCh];
int Ilim = 3;

int sortDC(const void *cmp1, const void *cmp2)
{
	int a = *(int *)cmp1;
	int b = *(int *)cmp2;

	return (channels[b].DC - channels[a].DC);
}

int sortDCRev(const void *cmp1, const void *cmp2)
{
	int a = *(int *)cmp1;
	int b = *(int *)cmp2;

	return (channels[a].DC - channels[b].DC);
}

int sortCur(const void *cmp1, const void *cmp2)
{
	tChData *a = (tChData *)cmp1;
	tChData *b = (tChData *)cmp2;

	return (b->maxCur - a->maxCur);
}


int main(int argc, char *argv[]) {
	int i,j,n,m;
	float total, totalGrpA, totalGrpB;
	int numGrpA, numGrpB;

	if (argc >= 5) {
		for(i=0;i<4;i++) {
			channels[i].maxCur = atof(argv[i+1]);
		}
	}

	if (argc == 9) {
		for(i=0;i<4;i++) {
			channels[i].DC = atoi(argv[i+5]);
		}
	}

	for(i=0;i<numCh;i++) {
		grpA[i] = -1;
		grpB[i] = -1;
	}

	qsort(channels, numCh, sizeof(channels[0]), sortCur);

	total = totalGrpA = totalGrpB = 0;
	numGrpA = numGrpB = 0;
	for(i=0;i<numCh;i++) {
		channels[i].newDC = channels[i].DC;
		total += channels[i].maxCur;
		if (totalGrpA + channels[i].maxCur <= Ilim) {
			totalGrpA += channels[i].maxCur;
			grpA[numGrpA++] = i;
		} else if (totalGrpB + channels[i].maxCur <= Ilim) {
			totalGrpB += channels[i].maxCur;
			grpB[numGrpB++] = i;
		}
	}

	qsort(grpA, numGrpA, sizeof(grpA[0]), sortDC);
	qsort(grpB, numGrpB, sizeof(grpB[0]), sortDCRev);

	printf("\n");
	for(i=0; i<numGrpA; i++) {
		if (grpB[i] > -1) {
			if (channels[grpA[i]].DC + channels[grpB[i]].DC > 100)
				channels[grpB[i]].newDC = 100 - channels[grpA[i]].DC;
		}
	}

	for (i=0;i<numCh;i++) {
		printf("Ch %d: phyCh = %d, maxCur = %f, DC = %d\n", i, channels[i].phyCh, channels[i].maxCur, channels[i].DC);
	}
	printf("Total = %f\n", total);
	printf("numGrpA = %d\n", numGrpA);
	printf("numGrpB = %d\n", numGrpB);
	printf("totalGrpA = %f\n", totalGrpA);
	printf("totalGrpB = %f\n", totalGrpB);

	for (i=0;i<numGrpA;i++) {
		printf("GrpA[%d] = Ch %d: maxCur = %f, DC = %d\n", i, grpA[i], channels[grpA[i]].maxCur, channels[grpA[i]].DC);
	}
	for (i=0;i<numGrpB;i++) {
		printf("GrpB[%d] = Ch %d: maxCur = %f, DC = %d -> %d\n", i, grpB[i], channels[grpB[i]].maxCur, channels[grpB[i]].DC, channels[grpB[i]].newDC);
	}

}
