#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>


struct msgbuf
{
	long mtype;
	int*  data;
};

enum Messages
{
	JUDGE_TYPE  = 1024,

	RUNNER_HERE = 1,
};


struct msgbuf CreateMsgbuf (long type, int* data);
int Runner (int n_runners, int runner_no, int msg_id);
int Judge (int n_runners, int msg_id);
int Receive (int msg_id, long mtype);
int Send (int msg_id, long mtype);


int main (int argc, char** argv)
{
	printf ("GOD:  I CREATE THE RUNHOUSE\n");

	if (argc != 2)
	{
		printf ("Please enter only 1 number - number of runners\n");
		return 1;
	}

	int n_runners = 0;
	sscanf (argv[1], "%d", &n_runners);

	int msg_id = msgget(IPC_PRIVATE, 0777 | IPC_CREAT);

	int child_pid = fork ();

	if (child_pid == 0)
	{
		return Judge (n_runners, msg_id);
	}

	for (int i = 1; i < n_runners + 1; i++)
	{
		child_pid = fork ();

		if (child_pid == 0)
		{
			return Runner (n_runners, i, msg_id);
		}
	}

	for (int i = 0; i < n_runners + 1; i++)
		wait (NULL);

	printf ("GOD: NOW I KILL THE RUNHOUSE\n");
	return 0;
}

int Judge (int n_runners, int msg_id)
{
	for (int i = 1; i < n_runners + 1; i++)
	{
		Receive (msg_id, i);

		printf ("JUDGE:  RUNNER%d has arrived!\n", i);
	}

	printf ("JUDGE:  Everyone is here!!! Let's begin the gonka!\n");

	for (int i = 0; i < n_runners; i++)
		Send (msg_id, JUDGE_TYPE);															

	Receive (msg_id, n_runners);

	printf ("JUDGE:  The gonka is over!!! Bye-Bye!!!\n");

	for (int i = 0; i < n_runners; i++)
		Send (msg_id, JUDGE_TYPE);

	return 0;
}


int Runner (int n_runners, int runner_no, int msg_id)
{
	printf ("RUNNER%d:  O hi, I'm here\n", runner_no);

	Send (msg_id, runner_no);

	Receive (msg_id, JUDGE_TYPE);

	if (runner_no == 1)
		Send (msg_id, runner_no);
	else
		Receive (msg_id, runner_no - 1);

	printf ("RUNNER%d:  RUNNING RUNNING RUNNING...\n", runner_no);
	sleep (1);

	Send (msg_id, runner_no);

	Receive (msg_id, JUDGE_TYPE);

	printf ("RUNNER%d:  Thank you for the race!!! Bye-Bye!!!\n", runner_no);

	return 0;

}


struct msgbuf CreateMsgbuf (long type, int* data)
{
	if (type <= 0)
	{
		printf ("ERROR: mtype must be > 0!\n");
	}

	struct msgbuf msg = {type, data};

	return msg;
}


int Send (int msg_id, long mtype)
{
	int msg_data = 0;

	struct msgbuf msg = CreateMsgbuf (mtype, &msg_data);

	int msg_bytes = msgsnd (msg_id, &msg, sizeof (int), 0);

	if (msg_bytes == -1)
	{
		perror ("OH NO! ERROR!...\n");
		return 1;
	}

	return 0;
}


int Receive (int msg_id, long mtype)
{
	int msg_data = 0;

	struct msgbuf msg = CreateMsgbuf (1, &msg_data);

	size_t msg_bytes = msgrcv (msg_id, &msg, sizeof (int), mtype, 0);

	if (msg_bytes == -1)
	{
		perror ("OH NO! ERROR!...\n");
		return 1;
	}

	return 0;
}