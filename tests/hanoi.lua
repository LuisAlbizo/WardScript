function hanoi(disks, ini, aux, fin)
	if disks == 1 then
		print(ini, fin)
	else
		hanoi(disks - 1, ini, fin, aux)
		print(ini, fin)
		hanoi(disks - 1, aux, ini, fin)
	end
end


hanoi(16, 1, 2, 3)

