/*
 * ------------------------------------------------------------
 *  Electricity Bill Estimator
 *  This sample program is used as the input source for the
 *  Mini C Compiler lab project. It intentionally contains
 *  variables, input statements, nested arithmetic expressions,
 *  string literals, character literals, and both comment styles
 *  so every phase of the compiler has something real to work on.
 * ------------------------------------------------------------
 */

#include <stdio.h>

int main() {
    int units;              // number of electricity units consumed
    float rate = 6.5;       // price per unit in local currency
    float surcharge;        // extra government surcharge
    float totalBill;        // final amount to be paid
    char grade;              /* billing category: 'A', 'B' or 'C' */
    char customerName[30];   // name of the customer

    printf("Enter customer name: ");   // ask for the customer's name
    scanf("%s", customerName);

    printf("Enter number of units consumed: ");
    scanf("%d", &units);

    /* The surcharge is calculated using a nested arithmetic
       expression that mixes addition, subtraction, multiplication
       and division inside parentheses. */
    surcharge = (units * rate) / (10 - 2) + 25;

    // Total bill combines the base usage cost and the surcharge
    totalBill = (units * rate) + surcharge - 5;

    // Decide the billing grade based on simple thresholds
    if (units > 500) {
        grade = 'A';         // heavy usage
    } else if (units > 100) {
        grade = 'B';         // moderate usage
    } else {
        grade = 'C';         // light usage
    }

    printf("------------------------------\n");
    printf("Customer: %s\n", customerName);
    printf("Units Consumed: %d\n", units);
    printf("Surcharge: %f\n", surcharge);
    printf("Total Bill: %f\n", totalBill);
    printf("Billing Grade: %c\n", grade);
    printf("------------------------------\n");

    return 0; // end of program
}
