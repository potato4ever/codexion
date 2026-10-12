
int ft_atoi(char *s){
    int i = 0;
    int result = 0;
    while (s[i] >= '0' && s[i] <= '9')
    {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    if (s[i])
        return -1;
    return result;

}
